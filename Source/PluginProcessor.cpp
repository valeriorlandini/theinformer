/******************************************************************************
This file is part of The Informer.
Copyright 2024-2025 Valerio Orlandini <valeriorlandini@gmail.com>.

The Informer is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

The Informer is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
The Informer. If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <iostream>

std::atomic<int> TheInformerAudioProcessor::instanceCounter{ 0 };

TheInformerAudioProcessor::TheInformerAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                   .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                   .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ),
#endif
    treeState(*this, nullptr, juce::Identifier("InformerParameters"),
{
    std::make_unique<juce::AudioParameterInt>("port", "Port", 0, 65535, 9000),
    std::make_unique<juce::AudioParameterInt>("ip1", "IP address Octet 1", 0, 255, 127),
    std::make_unique<juce::AudioParameterInt>("ip2", "IP address Octet 2", 0, 255, 0),
    std::make_unique<juce::AudioParameterInt>("ip3", "IP address Octet 3", 0, 255, 0),
    std::make_unique<juce::AudioParameterInt>("ip4", "IP address Octet 4", 0, 255, 1),
    std::make_unique<juce::AudioParameterBool>("normalize", "Normalize Values", false),
}),
fftProcessorSmall(orderSmall),
windowSmall(fftSizeSmall, juce::dsp::WindowingFunction<float>::hann),
fftProcessorLarge(orderLarge),
windowLarge(fftSizeLarge, juce::dsp::WindowingFunction<float>::hann)
{
    instance = ++instanceCounter;

    if (!treeState.state.hasProperty("root"))
    {
        treeState.state.setProperty("root", "informer_" + juce::String(instance), nullptr);
    }

    rootValue.referTo(treeState.state.getPropertyAsValue("root", nullptr));

    portParameter = treeState.getRawParameterValue("port");
    port = int(*portParameter);

    for (unsigned int i = 0; i < 4; i++)
    {
        ipParameters[i] = treeState.getRawParameterValue("ip" + std::to_string(i+1));
    }
    host = makeHost();

    normParameter = treeState.getRawParameterValue("normalize");
}

TheInformerAudioProcessor::~TheInformerAudioProcessor()
{
    --instanceCounter;
}

const juce::String TheInformerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TheInformerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TheInformerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TheInformerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TheInformerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TheInformerAudioProcessor::getNumPrograms()
{
    return 1;
}

int TheInformerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TheInformerAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String TheInformerAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);

    return {};
}

void TheInformerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void TheInformerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (sampleRate > 48000.0)
    {
        fftSize = static_cast<unsigned int>(fftSizeLarge);
        fftProcessor = fftProcessorLarge;
        window = windowLarge;
    }
    else
    {
        fftSize = static_cast<unsigned int>(fftSizeSmall);
        fftProcessor = fftProcessorSmall;
        window = windowSmall;
    }

    updateBlocks = fftSize / (samplesPerBlock * 2);
    if (updateBlocks == 0)
    {
        ++updateBlocks;
    }
    expectedSamples = samplesPerBlock * updateBlocks * 2;
    counter = 0;
    for (auto &s : samples)
    {
        s.clear();
    }

    fftBandwidth = static_cast<float>(sampleRate) / static_cast<float>(fftSize);
    frequencies.fill(0.0f);
    for (auto b = 0; b < fftSize / 2; b++)
    {
        frequencies.at(static_cast<unsigned int>(b)) = b * fftBandwidth;
    }

    if (sampleRate > 0.0)
    {
        invNyquist = 1.0f / (static_cast<float>(sampleRate) * 0.5f);        
    }
}

void TheInformerAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TheInformerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
}
#endif

void TheInformerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::ignoreUnused(midiMessages);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (auto channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++)
        {
            if (channel < 64)
            {
                sqrGains[(unsigned int)channel] += channelData[i] * channelData[i];
                samples.at((unsigned int)channel).push_back(channelData[i]);
            }
        }
    }

    if (++counter >= updateBlocks)
    {
        float ampKurtosis = 0.0f;
        std::vector<float> ampKurtoses;

        float ampPeak = 0.0f;
        std::vector<float> ampPeaks;

        float rms = 0.0f;
        std::vector<float> chRms;

        float ampSkewness = 0.0f;
        std::vector<float> ampSkewnesses;

        float variance = 0.0f;
        std::vector<float> variances;

        float zerocrossing = 0.0f;
        std::vector<float> zerocrossings;

        float centroid = 0.0f;
        std::vector<float> centroids;

        float decrease = 0.0f;
        std::vector<float> decreases;

        float entropy = 0.0f;
        std::vector<float> entropies;

        float flatness = 0.0f;
        std::vector<float> flatnesses;

        float flux = 0.0f;
        std::vector<float> fluxes;

        float irregularity = 0.0f;
        std::vector<float> irregularities;

        float kurtosis = 0.0f;
        std::vector<float> kurtoses;

        float peak = 0.0f;
        std::vector<float> peaks;

        float rolloff = 0.0f;
        std::vector<float> rolloffs;

        float scf = 0.0f;
        std::vector<float> scfs;

        float skewness = 0.0f;
        std::vector<float> skewnesses;

        float slope = 0.0f;
        std::vector<float> slopes;

        float spread = 0.0f;
        std::vector<float> spreads;

        for (unsigned int ch = 0; ch < (unsigned int)std::min(totalNumInputChannels, 64); ch++)
        {
            /* AMPLITUDE DESCRIPTORS */

            float frameSamples = static_cast<float>(buffer.getNumSamples()) * static_cast<float>(counter);

            chRms.push_back(std::sqrt(sqrGains[ch] / frameSamples));
            rms += chRms.at(ch);
            sqrGains[ch] = 0.0f;

            float mean = 0.0f;
            float chAmpPeak = 0.0f;
            float chZerocrossing = 0.0f;
            for (unsigned int n = 0; n < samples.at(ch).size(); n++)
            {   
                float s = samples.at(ch).at(n);
                mean += s;
                
                if (n > 0)
                {
                    chZerocrossing += std::fabs(static_cast<float>(!std::signbit(s)) - static_cast<float>(!std::signbit(samples.at(ch).at(n - 1))));
                }
                
                if (abs(s) > chAmpPeak)
                {
                    chAmpPeak = abs(s);
                    if (chAmpPeak > ampPeak)
                    {
                        ampPeak = chAmpPeak;
                    }
                }
            }

            mean /= samples.at(ch).size();
            chZerocrossing /= static_cast<float>(std::max(0, static_cast<int>(samples.at(ch).size()) - 1));
            float chVariance = 0.0f;
            float chAmpKurtosis = 0.0f;
            float chAmpSkewness = 0.0f;
            for (auto const& s : samples.at(ch))
            {
                chVariance += powf(s - mean, 2.0f);
            }
            chVariance /= frameSamples;

            float invSqrChVariance = chVariance * chVariance;

            // If variance is (almost or equal to) 0, invSqrVariance is left
            // to 0 so that kurtosis and skewness are not undefined
            if (abs(invSqrChVariance) > 0.00001f)
            {
                invSqrChVariance = 1.0f / invSqrChVariance;
                for (auto const& s : samples.at(ch))
                {
                    chAmpKurtosis += powf(s - mean, 4.0f);
                    chAmpSkewness += powf(s - mean, 3.0f);
                }

                chAmpKurtosis /= frameSamples;
                chAmpKurtosis *= invSqrChVariance;
                chAmpKurtosis -= 3.0f;

                chAmpSkewness /= frameSamples;
                chAmpSkewness /= powf(sqrt(chVariance), 3.0f);
            }
            else
            {
                chAmpKurtosis = 0.0f;
                chAmpSkewness = 0.0f;
            }

            ampKurtoses.push_back(chAmpKurtosis);
            ampKurtosis += chAmpKurtosis;

            ampPeaks.push_back(chAmpPeak);

            ampSkewnesses.push_back(chAmpSkewness);
            ampSkewness += chAmpSkewness;

            variances.push_back(chVariance);
            variance += chVariance;

            zerocrossings.push_back(chZerocrossing);
            zerocrossing += chZerocrossing;

            /* SPECTRAL DESCRIPTORS */

            fftData.at(ch).clear();
            fftData.at(ch).resize(static_cast<unsigned int>(fftSize) * 2);
            for (unsigned int s = 0; s < static_cast<unsigned int>(std::min(static_cast<int>(samples.at(ch).size()), fftSize)); s++)
            {
                fftData.at(ch).at(s) = samples.at(ch).at(s);
            }
            window.get().multiplyWithWindowingTable(fftData.at(ch).data(), static_cast<unsigned int>(fftSize));
            fftProcessor.get().performFrequencyOnlyForwardTransform(fftData.at(ch).data());

            float a = 0.0f;
            float chCentroid = 0.0f;
            float chDecrease = 0.0f;
            float chEntropy = 0.0f;
            float chFlatness = 0.0f;
            float chFlux = 0.0f;
            float chIrregularity = 0.0f;
            float chKurtosis = 0.0f;
            float chRolloff = 0.0f;
            float chScf = 0.0f;
            float chSkewness = 0.0f;
            float chSlope = 0.0f;
            float chSpread = 0.0f;
            float cumulPower = 0.0f;
            float freqSum = 0.0f;
            float maxMagnitude = 0.0f;
            float magnLnSum = 0.0f;
            float magnSum = 0.0f;
            std::vector<float> power;
            float powerSum = 0.0f;
            unsigned int fftHalf = static_cast<unsigned int>(fftSize) / 2;
            for (unsigned int k = 0; k < fftHalf; k++)
            {
                prev_magnitudes.at(ch).at(k) = magnitudes.at(ch).at(k);
                magnitudes.at(ch).at(k) = fabs(fftData.at(ch).at(k));
            }

            for (unsigned int k = 0; k < fftHalf; k++)
            {
                float magnitude = magnitudes.at(ch).at(k);
                float prev_magnitude = prev_magnitudes.at(ch).at(k);
                power.push_back(magnitude * magnitude);
                powerSum += power.at(k);
                if (magnitude > maxMagnitude)
                {
                    maxMagnitude = magnitude;
                }
                float frequency = frequencies.at(k);
                freqSum += frequency;

                a += frequency * magnitude;
                magnSum += magnitude;

                if (magnitude > 0.0f)
                {
                    magnLnSum += std::log(magnitude);
                }
                else
                {
                    magnLnSum += std::log(0.00001f);
                }

                chFlux += std::powf(magnitude - prev_magnitude, 2.0f);
            }
            chFlux = std::sqrtf(chFlux);
            if (powerSum > 0.0f)
            {
                for (unsigned int k = 0; k < fftHalf; k++)
                {
                    if (power.at(k) > 0.0f)
                    {
                        float powerNorm = power.at(k) / powerSum;
                        chEntropy += powerNorm * std::log2(powerNorm);
                    }
                }
            }
            chEntropy *= -1.0f;
            chEntropy /= std::log2(static_cast<float>(fftHalf));
            if (abs(magnSum) > 0.00001f)
            {
                chCentroid = a / magnSum;
                chScf = maxMagnitude / magnSum;

                float magnMean = magnSum / static_cast<float>(fftHalf);
                float magnGeoMean = std::exp(magnLnSum / static_cast<float>(fftHalf));
                chFlatness = magnGeoMean / magnMean;

                float chSlopeNum = 0.0f;
                float chSlopeDen = 0.0f;
                float freqMean = freqSum / static_cast<float>(fftHalf);
                for (unsigned int k = 0; k < fftHalf; k++)
                {
                    chSlopeNum += (frequencies.at(k) - freqMean) * (magnitudes.at(ch).at(k) - magnMean);
                    chSlopeDen += (frequencies.at(k) - freqMean) * (frequencies.at(k) - freqMean);
                }
                chSlope = chSlopeNum / chSlopeDen;
                // Multiply by 100 to have a range closer to [-1.0, 1.0]
                chSlope *= 100.0f;
            }

            float rolloffThresh = powerSum * 0.85f;
            bool threshReached = false;
            a = 0.0f;
            for (unsigned int k = 0; k < fftHalf; k++)
            {
                float frequency = frequencies.at(k);
                a += magnitudes.at(ch).at(k) * std::powf(frequency - chCentroid, 2.0);

                if (cumulPower < rolloffThresh)
                {
                    cumulPower += power.at(k);
                }
                if (cumulPower >= rolloffThresh && !threshReached)
                {
                    threshReached = true;
                    chRolloff = frequencies.at(k);
                }

                if (k > 0)
                {
                    chDecrease += (magnitudes.at(ch).at(k) - magnitudes.at(ch).at(0)) / static_cast<float>(k);
                    chIrregularity += std::fabs(magnitudes.at(ch).at(k) - magnitudes.at(ch).at(k - 1));
                }
            }
            if (abs(powerSum) > 0.00001f)
            {
                chSpread = std::sqrtf(a / magnSum);
            }

            float magnSumNoDC = magnSum - magnitudes.at(ch).at(0);
            if (magnSumNoDC > 0.0f)
            {
                chDecrease /= magnSumNoDC;
            }
            else
            {
                decrease = 0.0f;
            }

            if (magnSum > 0.0f)
            {
                chIrregularity /= magnSum;
            }
            else
            {
                chIrregularity = 0.0f;
            }

            float kurtNum = 0.0f;
            float kurtDen = 0.0f;
            float skewNum = 0.0f;
            float skewDen = 0.0f;
            for (unsigned int k = 0; k < fftHalf; k++)
            {
                float frequency = frequencies.at(k);
                float magnitude = magnitudes.at(ch).at(k);

                kurtNum += std::powf(frequency - chCentroid, 4.0f) * magnitude;
                skewNum += std::powf(frequency - chCentroid, 3.0f) * magnitude;
                skewDen += magnitude; 
            }
            kurtDen = skewDen * std::powf(chSpread, 4.0f);
            skewDen *= std::powf(chSpread, 3.0f);
            if (abs(skewDen) > 0.00001f)
            {
                chSkewness = skewNum / skewDen;
                chKurtosis = (kurtNum / kurtDen) - 3.0f;
            }

            auto peakBand = std::max_element(fftData.at(ch).begin(), fftData.at(ch).end());
            float chPeak = static_cast<float>(std::distance(fftData.at(ch).begin(), peakBand)) * fftBandwidth;

            centroids.push_back(chCentroid);
            centroid += chCentroid;
            decreases.push_back(chDecrease);
            decrease += chDecrease;
            entropies.push_back(chEntropy);
            entropy += chEntropy;
            flatnesses.push_back(chFlatness);
            flatness += chFlatness;
            fluxes.push_back(chFlux);
            flux += chFlux;
            peaks.push_back(chPeak);
            peak += chPeak;
            irregularities.push_back(chIrregularity);
            irregularity += chIrregularity;
            kurtoses.push_back(chKurtosis);
            kurtosis += chKurtosis;
            rolloffs.push_back(chRolloff);
            rolloff += chRolloff;
            scfs.push_back(chScf);
            scf += chScf;
            skewnesses.push_back(chSkewness);
            skewness += chSkewness;
            slopes.push_back(chSlope);
            slope += chSlope;
            spreads.push_back(chSpread);
            spread += chSpread;
        }

        centroid /= totalNumInputChannels;
        decrease /= totalNumInputChannels;
        entropy /= totalNumInputChannels;
        flatness /= totalNumInputChannels;
        flux /= totalNumInputChannels;
        peak /= totalNumInputChannels;
        irregularity /= totalNumInputChannels;
        ampKurtosis /= totalNumInputChannels;
        kurtosis /= totalNumInputChannels;
        rolloff /= totalNumInputChannels;
        rms /= totalNumInputChannels;
        scf /= totalNumInputChannels;
        ampSkewness /= totalNumInputChannels;
        skewness /= totalNumInputChannels;
        slope /= totalNumInputChannels;
        spread /= totalNumInputChannels;
        variance /= totalNumInputChannels;
        zerocrossing /= totalNumInputChannels;

        if (*normParameter > 0.5f)
        {
            ampKurtosis += 2.0f;
            ampKurtosis = std::clamp(ampKurtosis, 0.0f, 4.0f);
            ampKurtosis *= 0.25f;
            for (float& k : ampKurtoses)
            {
                k += 2.0f;
                k = std::clamp(k, 0.0f, 4.0f);
                k *= 0.25f;
            }

            kurtosis += 2.0f;
            kurtosis = std::clamp(kurtosis, 0.0f, 4.0f);
            kurtosis *= 0.25f;
            for (float& k : kurtoses)
            {
                k += 2.0f;
                k = std::clamp(k, 0.0f, 4.0f);
                k *= 0.25f;
            }

            centroid *= invNyquist;
            for (float& c : centroids)
            {
                c *= invNyquist;
            }

            decrease += 0.05f;
            decrease *= 10.0f;
            decrease = std::clamp(decrease, 0.0f, 1.0f);
            for (float& d : decreases)
            {
                d += 0.05f;
                d *= 10.0f;
                d = std::clamp(d, 0.0f, 1.0f);
            }

            flux /= static_cast<float>(fftSize) * 0.5f;
            flux = std::clamp(flux, 0.0f, 1.0f);
            for (float& f : fluxes)
            {
                f /= static_cast<float>(fftSize) * 0.5f;
                f = std::clamp(f, 0.0f, 1.0f);
            }

            peak *= invNyquist;
            for (float& f : peaks)
            {
                f *= invNyquist;
            }

            irregularity *= 0.5f;
            irregularity = std::clamp(irregularity, 0.0f, 1.0f);
            for (float& i : irregularities)
            {
                i *= 0.5f;
                i = std::clamp(i, 0.0f, 1.0f);
            }

            rolloff *= invNyquist;
            for (float& r : rolloffs)
            {
                r *= invNyquist;
            }

            ampSkewness = std::clamp(ampSkewness, -5.0f, 5.0f);
            ampSkewness += 5.0f;
            ampSkewness *= 0.1f;
            for (float& s : ampSkewnesses)
            {
                s = std::clamp(s, -5.0f, 5.0f);
                s += 5.0f;
                s *= 0.1f;
            }

            skewness = std::clamp(skewness, -5.0f, 5.0f);
            skewness += 5.0f;
            skewness *= 0.1f;
            for (float& s : skewnesses)
            {
                s = std::clamp(s, -5.0f, 5.0f);
                s += 5.0f;
                s *= 0.1f;
            }

            slope += 1.0f;
            slope *= 0.5;
            slope = std::clamp(slope, 0.0f, 1.0f);
            for (float& s : slopes)
            {
                s += 1.0f;
                s *= 0.5;
                s = std::clamp(s, 0.0f, 1.0f);
            }

            spread *= invNyquist * 0.5f;
            for (float& s : spreads)
            {
                s *= invNyquist * 0.5f;
            }
        }

        for (unsigned int i = 0; i < 64; i++)
        {
            // Erase the first half of the samples if it is not the first cycle
            if (samples.at(i).size() >= static_cast<unsigned int>(expectedSamples))
            {
                samples.at(i).erase(samples.at(i).begin(), samples.at(i).begin() + expectedSamples / 2);
            }
        }

        juce::String root = "/" + rootValue.toString() + "/";

        std::function<void()> reportStats = [centroid, centroids,
                                             decrease, decreases,
                                             entropy, entropies,
                                             flatness, flatnesses,
                                             flux, fluxes,
                                             ampPeak, ampPeaks,
                                             peak, peaks,
                                             irregularity, irregularities,
                                             ampKurtosis, ampKurtoses,
                                             kurtosis, kurtoses,
                                             rms, chRms,
                                             rolloff, rolloffs,
                                             scf, scfs,
                                             ampSkewness, ampSkewnesses,
                                             skewness, skewnesses,
                                             slope, slopes,
                                             spread, spreads,
                                             variance, variances,
                                             zerocrossing, zerocrossings,
                                             host = makeHost(),
                                             port = int(*portParameter),
                                             root]() mutable
        {
            juce::OSCSender sender;
            sender.connect(host, port);

            juce::String mix = "mix/";
            juce::String time = "time/";
            juce::String freq = "freq/";

            sender.send(juce::OSCAddressPattern(root + mix + time + "kurtosis"), ampKurtosis);
            sender.send(juce::OSCAddressPattern(root + mix + time + "peak"), ampPeak);
            sender.send(juce::OSCAddressPattern(root + mix + time + "rms"), rms);
            sender.send(juce::OSCAddressPattern(root + mix + time + "skewness"), ampSkewness);
            sender.send(juce::OSCAddressPattern(root + mix + time + "variance"), variance);
            sender.send(juce::OSCAddressPattern(root + mix + time + "zerocrossing"), zerocrossing);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "centroid"), centroid);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "decrease"), decrease);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "entropy"), entropy);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "flatness"), flatness);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "flux"), flux);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "peak"), peak);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "irregularity"), irregularity);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "kurtosis"), kurtosis);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "rolloff"), rolloff);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "scf"), scf);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "skewness"), skewness);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "slope"), slope);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "spread"), spread);

            for (unsigned int ch = 0; ch < chRms.size(); ch++)
            {
                std::string ch_str = "ch";
                if (ch < 9)
                {
                    ch_str += "0";
                }
                ch_str += std::to_string(ch + 1) + "/";

                sender.send(juce::OSCAddressPattern(root + ch_str + time + "kurtosis"), ampKurtoses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "peak"), ampPeaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "rms"), chRms.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "skewness"), ampSkewnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "variance"), variances.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "zerocrossing"), zerocrossings.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "centroid"), centroids.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "decrease"), decreases.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "entropy"), entropies.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "flatness"), flatnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "flux"), fluxes.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "peak"), peaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "irregularity"), irregularities.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "kurtosis"), kurtoses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "rolloff"), rolloffs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "scf"), scfs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "skewness"), skewnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "slope"), slopes.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "spread"), spreads.at(ch));
            }

            sender.disconnect();
        };
        juce::MessageManager::callAsync(reportStats);

        counter = 0;
    }
}

bool TheInformerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* TheInformerAudioProcessor::createEditor()
{
    return new TheInformerAudioProcessorEditor(*this);
}

void TheInformerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(treeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TheInformerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(treeState.state.getType()))
        {
            treeState.state = juce::ValueTree::fromXml(*xmlState);

            if (treeState.state.hasProperty("root"))
            {
                rootValue.referTo(treeState.state.getPropertyAsValue("root", nullptr));
            }
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheInformerAudioProcessor();
}
