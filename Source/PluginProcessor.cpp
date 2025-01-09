/******************************************************************************
This file is part of The Informer.
Copyright 2024 Valerio Orlandini <valeriorlandini@gmail.com>.

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
    updateBlocks = fftSize / samplesPerBlock;
    if (updateBlocks == 0)
    {
        updateBlocks++;
    }

    fftBandwidth = static_cast<float>(sampleRate) / static_cast<float>(fftSize);
    frequencies.fill(0.0f);
    for (auto b = 0; b < fftSize / 2; b++)
    {
        frequencies.at(static_cast<unsigned int>(b)) = b * fftBandwidth;
    }

    if (sampleRate > 0.0)
    {
        invNyquist = 1.0f / static_cast<float>(sampleRate);        
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

    if (++counter > updateBlocks)
    {
        float peak = 0.0f;
        std::vector<float> peaks;

        float rms = 0.0f;
        std::vector<float> chRms;

        float variance = 0.0f;
        std::vector<float> variances;

        float kurtosis = 0.0f;
        std::vector<float> kurtoses;

        float centroid = 0.0f;
        std::vector<float> centroids;

        float entropy = 0.0f;
        std::vector<float> entropies;

        float flatness = 0.0f;
        std::vector<float> flatnesses;

        float flux = 0.0f;
        std::vector<float> fluxes;

        float fpeak = 0.0f;
        std::vector<float> fpeaks;

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

            chRms.push_back(std::sqrt(sqrGains[ch] / (buffer.getNumSamples() * static_cast<float>(counter))));
            rms += chRms.at(ch);
            sqrGains[ch] = 0.0f;

            float mean = 0.0f;
            float chPeak = 0.0f;
            for (auto const& s : samples.at(ch))
            {
                mean += s;
                if (abs(s) > chPeak)
                {
                    chPeak = abs(s);
                    if (chPeak > peak)
                    {
                        peak = chPeak;
                    }
                }
            }

            mean /= samples.at(ch).size();
            float chVariance = 0.0f;
            float chKurtosis = 0.0f;
            for (auto const& s : samples.at(ch))
            {
                chVariance += powf(s - mean, 2.0f);
            }
            chVariance /= buffer.getNumSamples() * static_cast<float>(counter);

            float invSqrChVariance = chVariance * chVariance;

            // If variance is (almost or equal to) 0, invSqrVariance is left
            // to 0 so that kurtosis and skewness are not undefined
            if (abs(invSqrChVariance) > 0.00001f)
            {
                invSqrChVariance = 1.0f / invSqrChVariance;
                for (auto const& s : samples.at(ch))
                {
                    chKurtosis += powf(s - mean, 4.0f) * invSqrChVariance;
                }

                chKurtosis /= buffer.getNumSamples() * static_cast<float>(counter);
                chKurtosis -= 3.0f;
            }
            else
            {
                chKurtosis = 0.0f;
            }

            kurtoses.push_back(chKurtosis);
            kurtosis += chKurtosis;

            peaks.push_back(chPeak);

            variances.push_back(chVariance);
            variance += chVariance;

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
            float chEntropy = 0.0f;
            float chFlatness = 0.0f;
            float chFlux = 0.0f;
            float chRolloff = 0.0f;
            float chScf = 0.0f;
            float chSkewness = 0.0f;
            float chSlope = 0.0f;
            float chSpread = 0.0f;
            float cumulPower = 0.0f;
            float freqSum = 0.0f;
            float freqSqSum = 0.0f;
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
                freqSqSum += frequency * frequency;

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
                        chEntropy += powerNorm * std::log(powerNorm);
                    }
                }
            }
            chEntropy *= -1.0f;
            chEntropy /= std::log(static_cast<float>(fftHalf));
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
                a += power.at(k) * std::powf(frequency - chCentroid, 2.0);

                if (cumulPower < rolloffThresh)
                {
                    cumulPower += power.at(k);
                }
                if (cumulPower >= rolloffThresh && !threshReached)
                {
                    threshReached = true;
                    chRolloff = frequencies.at(k);
                }
            }
            if (abs(powerSum) > 0.00001f)
            {
                chSpread = std::sqrtf(a / powerSum);
            }

            float skewNum = 0.0f;
            float skewDen = 0.0f;
            for (unsigned int k = 0; k < fftHalf; k++)
            {
                float frequency = frequencies.at(k);
                float magnitude = magnitudes.at(ch).at(k);

                skewNum += std::powf(frequency - chCentroid, 3.0f) * magnitude;
                skewDen += magnitude; 
            }
            skewDen *= std::powf(chSpread, 3.0f);
            if (abs(skewDen) > 0.00001f)
            {
                chSkewness = skewNum / skewDen;
            }

            auto fpeakBand = std::max_element(fftData.at(ch).begin(), fftData.at(ch).end());
            float chFpeak = static_cast<float>(std::distance(fftData.at(ch).begin(), fpeakBand)) * fftBandwidth;

            centroids.push_back(chCentroid);
            centroid += chCentroid;
            entropies.push_back(chEntropy);
            entropy += chEntropy;
            flatnesses.push_back(chFlatness);
            flatness += chFlatness;
            fluxes.push_back(chFlux);
            flux += chFlux;
            fpeaks.push_back(chFpeak);
            fpeak += chFpeak;
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
        entropy /= totalNumInputChannels;
        flatness /= totalNumInputChannels;
        flux /= totalNumInputChannels;
        fpeak /= totalNumInputChannels;
        kurtosis /= totalNumInputChannels;
        rolloff /= totalNumInputChannels;
        rms /= totalNumInputChannels;
        scf /= totalNumInputChannels;
        skewness /= totalNumInputChannels;
        slope /= totalNumInputChannels;
        spread /= totalNumInputChannels;
        variance /= totalNumInputChannels;

        if (*normParameter > 0.5f)
        {
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

            flux /= static_cast<float>(fftSize) * 0.5f;
            flux = std::clamp(flux, 0.0f, 1.0f);
            for (float& f : fluxes)
            {
                f /= static_cast<float>(fftSize) * 0.5f;
                f = std::clamp(f, 0.0f, 1.0f);
            }

            fpeak *= invNyquist;
            for (float& f : fpeaks)
            {
                f *= invNyquist;
            }

            rolloff *= invNyquist;
            for (float& r : rolloffs)
            {
                r *= invNyquist;
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

            spread *= invNyquist;
            for (float& s : spreads)
            {
                s *= invNyquist;
            }
        }

        for (unsigned int i = 0; i < 64; i++)
        {
            samples.at(i).clear();
        }

        juce::String root = "/" + rootValue.toString() + "/";

        std::function<void()> reportStats = [centroid, centroids,
                                             entropy, entropies,
                                             flatness, flatnesses,
                                             flux, fluxes,
                                             fpeak, fpeaks,
                                             kurtosis, kurtoses,
                                             peak, peaks,
                                             rms, chRms,
                                             rolloff, rolloffs,
                                             scf, scfs,
                                             skewness, skewnesses,
                                             slope, slopes,
                                             spread, spreads,
                                             variance, variances,
                                             host = makeHost(),
                                             port = int(*portParameter),
                                             root]() mutable
        {
            juce::OSCSender sender;
            sender.connect(host, port);

            juce::String mix = "mix/";

            sender.send(juce::OSCAddressPattern(root + mix + "kurtosis"), kurtosis);
            sender.send(juce::OSCAddressPattern(root + mix + "peak"), peak);
            sender.send(juce::OSCAddressPattern(root + mix + "rms"), rms);
            sender.send(juce::OSCAddressPattern(root + mix + "variance"), variance);
            sender.send(juce::OSCAddressPattern(root + mix + "centroid"), centroid);
            sender.send(juce::OSCAddressPattern(root + mix + "entropy"), entropy);
            sender.send(juce::OSCAddressPattern(root + mix + "flatness"), flatness);
            sender.send(juce::OSCAddressPattern(root + mix + "flux"), flux);
            sender.send(juce::OSCAddressPattern(root + mix + "freqpeak"), fpeak);
            sender.send(juce::OSCAddressPattern(root + mix + "rolloff"), rolloff);
            sender.send(juce::OSCAddressPattern(root + mix + "scf"), scf);
            sender.send(juce::OSCAddressPattern(root + mix + "skewness"), skewness);
            sender.send(juce::OSCAddressPattern(root + mix + "slope"), slope);
            sender.send(juce::OSCAddressPattern(root + mix + "spread"), spread);

            for (unsigned int ch = 0; ch < chRms.size(); ch++)
            {
                std::string ch_str = "ch";
                if (ch < 9)
                {
                    ch_str += "0";
                }
                ch_str += std::to_string(ch + 1) + "/";

                sender.send(juce::OSCAddressPattern(root + ch_str + "kurtosis"), kurtoses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "peak"), peaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "rms"), chRms.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "variance"), variances.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "centroid"), centroids.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "entropy"), entropies.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "flatness"), flatnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "flux"), fluxes.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "freqpeak"), fpeaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "rolloff"), rolloffs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "scf"), scfs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "skewness"), skewnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "slope"), slopes.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "spread"), spreads.at(ch));
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
