/******************************************************************************
This file is part of The Informer.
Copyright 2024-2026 Valerio Orlandini <valeriorlandini@gmail.com>.

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
        std::make_unique<juce::AudioParameterBool>("smoothing", "Smooth Master Parameters", false),
        std::make_unique<juce::AudioParameterInt>("reportbands", "Report Bands", 2, 16, 3),
}),
fftProcessorSmall(orderSmall),
windowSmall(fftSizeSmall, juce::dsp::WindowingFunction<float>::hann),
fftProcessorLarge(orderLarge),
windowLarge(fftSizeLarge, juce::dsp::WindowingFunction<float>::hann),
ampKurtoses(64, 0.0f),
ampPeaks(64, 0.0f),
f0s(64, 0.0f),
chRms(64, 0.0f),
ampSkewnesses(64, 0.0f),
variances(64, 0.0f),
zerocrossings(64, 0.0f),
centroids(64, 0.0f),
decreases(64, 0.0f),
entropies(64, 0.0f),
flatnesses(64, 0.0f),
fluxes(64, 0.0f),
irregularities(64, 0.0f),
kurtoses(64, 0.0f),
peaks(64, 0.0f),
rolloffs(64, 0.0f),
scfs(64, 0.0f),
skewnesses(64, 0.0f),
slopes(64, 0.0f),
spreads(64, 0.0f)
{
    instance = ++instanceCounter;

    if (!treeState.state.hasProperty("root"))
    {
        treeState.state.setProperty("root", "informer_" + juce::String(instance), nullptr);
    }

    rootValue.referTo(treeState.state.getPropertyAsValue("root", nullptr));

    portParameter = treeState.getRawParameterValue("port");
    port = static_cast<int>(*portParameter);

    for (unsigned int i = 0; i < 4; i++)
    {
        ipParameters[i] = treeState.getRawParameterValue("ip" + std::to_string(i+1));
    }
    host = makeHost();

    normParameter = treeState.getRawParameterValue("normalize");
    smoothParameter = treeState.getRawParameterValue("smoothing");

    reportBandsParameter = treeState.getRawParameterValue("reportbands");
    reportBands = static_cast<unsigned int>(*reportBandsParameter);
    getEqualOctaveBandEdges();
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

    updateBlocks = fftSize / (static_cast<unsigned int>(samplesPerBlock) * 2u);
    if (updateBlocks == 0u)
    {
        ++updateBlocks;
    }
    expectedSamples = static_cast<unsigned int>(samplesPerBlock) * updateBlocks * 2u;
    counter = 0u;
    for (auto &s : samples)
    {
        s.clear();
    }

    fftBandwidth = static_cast<float>(sampleRate) / static_cast<float>(fftSize);
    frequencies.fill(0.0f);
    for (auto b = 0u; b < fftSize / 2u; b++)
    {
        frequencies.at(b) = static_cast<float>(b) * fftBandwidth;
    }

    if (sampleRate > 0.0)
    {
        invNyquist = 1.0f / (static_cast<float>(sampleRate) * 0.5f);
    }

    getEqualOctaveBandEdges();

    smoothingCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(1.0f / static_cast<float>(fftSize) * 2.0f, 2.5f);

    for (auto& f : smoothingFilters)
    {
        f.coefficients = smoothingCoefficients;
    }

    for (auto& d : computeDescriptors)
    {
        d.set_sample_rate(static_cast<float>(sampleRate));
        d.set_stft_size(fftSize);
    }

    for (auto& m : magnitudes)
    {
        m.resize(fftSize / 2u, 0.0f);
    }

    for (auto& m : prev_magnitudes)
    {
        m.resize(fftSize / 2u, 0.0f);
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
        ampKurtosis = 0.0f;
        ampPeak = 0.0f;
        rms = 0.0f;
        ampSkewness = 0.0f;
        variance = 0.0f;
        zerocrossing = 0.0f;

        centroid = 0.0f;
        decrease = 0.0f;
        entropy = 0.0f;
        flatness = 0.0f;
        flux = 0.0f;
        irregularity = 0.0f;
        kurtosis = 0.0f;
        peak = 0.0f;
        rolloff = 0.0f;
        scf = 0.0f;
        skewness = 0.0f;
        slope = 0.0f;
        spread = 0.0f;

        std::array<std::vector<float>, 65> bandMagnitudes;
        if (static_cast<unsigned int>(*reportBandsParameter) != reportBands)
        {
            reportBands = static_cast<unsigned int>(*reportBandsParameter);
            getEqualOctaveBandEdges();
        }
        if (reportBands > 0)
        {
            for (auto &m : bandMagnitudes)
            {
                m.resize(reportBands);
            }
        }

        for (unsigned int ch = 0; ch < static_cast<unsigned int>(std::min(totalNumInputChannels, 64)); ch++)
        {
            /* AMPLITUDE DESCRIPTORS */

            computeDescriptors.at(ch).set_buffer(samples.at(ch));
            computeDescriptors.at(ch).compute_descriptors(true, false);
            auto timeDescriptors = computeDescriptors.at(ch).get_time_descriptors();

            chRms[ch] = timeDescriptors.at("rms");
            rms += chRms.at(ch);

            ampKurtoses[ch] = timeDescriptors.at("kurtosis");
            ampKurtosis += ampKurtoses[ch];

            ampPeaks[ch] = timeDescriptors.at("peak");

            ampSkewnesses[ch] = timeDescriptors.at("skewness");
            ampSkewness += ampSkewnesses[ch];

            f0s[ch] = timeDescriptors.at("f0");
            f0 += f0s[ch];

            variances[ch] = timeDescriptors.at("variance");
            variance += variances[ch];

            zerocrossings[ch] = timeDescriptors.at("zerocrossing");
            zerocrossing += zerocrossings[ch];

            /* SPECTRAL DESCRIPTORS */

            fftData.at(ch).clear();
            fftData.at(ch).resize(static_cast<unsigned int>(fftSize) * 2u);
            for (unsigned int s = 0u; s < static_cast<unsigned int>(std::min(static_cast<unsigned int>(samples.at(ch).size()), fftSize)); s++)
            {
                fftData.at(ch).at(s) = samples.at(ch).at(s);
            }
            window.get().multiplyWithWindowingTable(fftData.at(ch).data(), static_cast<unsigned int>(fftSize));
            fftProcessor.get().performFrequencyOnlyForwardTransform(fftData.at(ch).data());

            unsigned int fftHalf = static_cast<unsigned int>(fftSize) / 2u;

            for (unsigned int k = 0; k < fftHalf; k++)
            {
                prev_magnitudes.at(ch).at(k) = magnitudes.at(ch).at(k);
                magnitudes.at(ch).at(k) = fabs(fftData.at(ch).at(k));
            }

            computeDescriptors.at(ch).set_previous_magnitudes(prev_magnitudes.at(ch));
            computeDescriptors.at(ch).set_magnitudes(magnitudes.at(ch));
            computeDescriptors.at(ch).compute_descriptors(false, true);
            auto spectralDescriptors = computeDescriptors.at(ch).get_frequency_descriptors();

            std::fill(bandMagnitudes.at(ch).begin(), bandMagnitudes.at(ch).end(), 0.0f);

            for (unsigned int k = 0u; k < fftHalf; k++)
            {
                float magnitude = magnitudes.at(ch).at(k);

                if (reportBands > 1u)
                {
                    // Find which reported band the current spectrogram band belongs to
                    unsigned int currRepBand = std::upper_bound(bandsEdges.begin(), bandsEdges.end(), frequencies[k]) - bandsEdges.begin() - 1u;
                    // If current magnitude is greater than maximum value stored in the corresponding reported band, update its value
                    bandMagnitudes.at(ch).at(currRepBand) = std::max(magnitude, bandMagnitudes.at(ch).at(currRepBand));
                }
            }

            // Scale magnitude values
            for (auto b = 0u; b < bandMagnitudes.at(ch).size(); b++)
            {
                bandMagnitudes.at(ch).at(b) *= 1.0f / static_cast<float>(fftHalf);
                bandMagnitudes.at(ch).at(b) = std::sqrt(std::min(1.0f, bandMagnitudes.at(ch).at(b)));
                bandMagnitudes.at(64).at(b) += bandMagnitudes.at(ch).at(b) / static_cast<float>(totalNumInputChannels);
            }

            centroids[ch] = spectralDescriptors.at("centroid");
            centroid += centroids[ch];
            decreases[ch] = spectralDescriptors.at("decrease");
            decrease += decreases[ch];
            entropies[ch] = spectralDescriptors.at("entropy");
            entropy += entropies[ch];
            flatnesses[ch] = spectralDescriptors.at("flatness");
            flatness += flatnesses[ch];
            fluxes[ch] = spectralDescriptors.at("flux");
            flux += fluxes[ch];
            peaks[ch] = spectralDescriptors.at("peak");
            peak += peaks[ch];
            irregularities[ch] = spectralDescriptors.at("irregularity");
            irregularity += irregularities[ch];
            kurtoses[ch] = spectralDescriptors.at("kurtosis");
            kurtosis += kurtoses[ch];
            rolloffs[ch] = spectralDescriptors.at("rolloff");
            rolloff += rolloffs[ch];
            scfs[ch] = spectralDescriptors.at("crestfactor");
            scf += scfs[ch];
            skewnesses[ch] = spectralDescriptors.at("skewness");
            skewness += skewnesses[ch];
            slopes[ch] = spectralDescriptors.at("slope");
            slope += slopes[ch];
            spreads[ch] = spectralDescriptors.at("spread");
            spread += spreads[ch];
        }

        centroid /= totalNumInputChannels;
        decrease /= totalNumInputChannels;
        entropy /= totalNumInputChannels;
        flatness /= totalNumInputChannels;
        flux /= totalNumInputChannels;
        f0 /= totalNumInputChannels;
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

            f0 *= invNyquist;
            for (float& f : f0s)
            {
                f *= invNyquist;
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

        for (unsigned int i = 0u; i < 64u; i++)
        {
            // Erase the first half of the samples if it is not the first cycle
            if (samples.at(i).size() >= static_cast<unsigned int>(expectedSamples))
            {
                samples.at(i).erase(samples.at(i).begin(), samples.at(i).begin() + expectedSamples / 2);
            }
        }

        juce::String root = "/" + rootValue.toString() + "/";

        auto reportCentroid = centroid;
        auto reportDecrease = decrease;
        auto reportEntropy = entropy;
        auto reportFlatness = flatness;
        auto reportFlux = flux;
        auto reportF0 = f0;
        auto reportAmpPeak = ampPeak;
        auto reportPeak = peak;
        auto reportIrregularity = irregularity;
        auto reportAmpKurtosis = ampKurtosis;
        auto reportKurtosis = kurtosis;
        auto reportRms = rms;
        auto reportRolloff = rolloff;
        auto reportScf = scf;
        auto reportAmpSkewness = ampSkewness;
        auto reportSkewness = skewness;
        auto reportSlope = slope;
        auto reportSpread = spread;
        auto reportVariance = variance;
        auto reportZerocrossing = zerocrossing;

        if (*smoothParameter > 0.5f)
        {
            reportCentroid = smoothingFilters[static_cast<size_t>(Descriptor::SpecCentroid)].processSample(centroid);
            reportDecrease = smoothingFilters[static_cast<size_t>(Descriptor::SpecDecrease)].processSample(decrease);
            reportEntropy = smoothingFilters[static_cast<size_t>(Descriptor::SpecEntropy)].processSample(entropy);
            reportFlatness = smoothingFilters[static_cast<size_t>(Descriptor::SpecFlatness)].processSample(flatness);
            reportFlux = smoothingFilters[static_cast<size_t>(Descriptor::SpecFlux)].processSample(flux);
            reportF0 = smoothingFilters[static_cast<size_t>(Descriptor::AmpPitch)].processSample(f0);
            reportAmpPeak = smoothingFilters[static_cast<size_t>(Descriptor::AmpPeak)].processSample(ampPeak);
            reportPeak = smoothingFilters[static_cast<size_t>(Descriptor::SpecPeak)].processSample(peak);
            reportIrregularity = smoothingFilters[static_cast<size_t>(Descriptor::SpecIrregularity)].processSample(irregularity);
            reportAmpKurtosis = smoothingFilters[static_cast<size_t>(Descriptor::AmpKurtosis)].processSample(ampKurtosis);
            reportKurtosis = smoothingFilters[static_cast<size_t>(Descriptor::SpecKurtosis)].processSample(kurtosis);
            reportRms = smoothingFilters[static_cast<size_t>(Descriptor::AmpRMS)].processSample(rms);
            reportRolloff = smoothingFilters[static_cast<size_t>(Descriptor::SpecRolloff)].processSample(rolloff);
            reportScf = smoothingFilters[static_cast<size_t>(Descriptor::SpecCrest)].processSample(scf);
            reportAmpSkewness = smoothingFilters[static_cast<size_t>(Descriptor::AmpSkewness)].processSample(ampSkewness);
            reportSkewness = smoothingFilters[static_cast<size_t>(Descriptor::SpecSkewness)].processSample(skewness);
            reportSlope = smoothingFilters[static_cast<size_t>(Descriptor::SpecSlope)].processSample(slope);
            reportSpread = smoothingFilters[static_cast<size_t>(Descriptor::SpecSpread)].processSample(spread);
            reportVariance = smoothingFilters[static_cast<size_t>(Descriptor::AmpVariance)].processSample(variance);
            reportZerocrossing = smoothingFilters[static_cast<size_t>(Descriptor::AmpZCR)].processSample(zerocrossing);
        }

        auto reportCentroids = centroids;
        auto reportDecreases = decreases;
        auto reportEntropies = entropies;
        auto reportFlatnesses = flatnesses;
        auto reportFluxes = fluxes;
        auto reportF0s = f0s;
        auto reportAmpPeaks = ampPeaks;
        auto reportPeaks = peaks;
        auto reportIrregularities = irregularities;
        auto reportAmpKurtoses = ampKurtoses;
        auto reportKurtoses = kurtoses;
        auto reportChRms = chRms;
        auto reportRolloffs = rolloffs;
        auto reportScfs = scfs;
        auto reportAmpSkewnesses = ampSkewnesses;
        auto reportSkewnesses = skewnesses;
        auto reportSlopes = slopes;
        auto reportSpreads = spreads;
        auto reportVariances = variances;
        auto reportZerocrossings = zerocrossings;

        std::function<void()> reportStats = [reportCentroid, reportCentroids,
                                             reportDecrease, reportDecreases,
                                             reportEntropy, reportEntropies,
                                             reportFlatness, reportFlatnesses,
                                             reportFlux, reportFluxes,
                                             reportF0, reportF0s,
                                             reportAmpPeak, reportAmpPeaks,
                                             reportPeak, reportPeaks,
                                             reportIrregularity, reportIrregularities,
                                             reportAmpKurtosis, reportAmpKurtoses,
                                             reportKurtosis, reportKurtoses,
                                             reportRms, reportChRms,
                                             reportRolloff, reportRolloffs,
                                             reportScf, reportScfs,
                                             reportAmpSkewness, reportAmpSkewnesses,
                                             reportSkewness, reportSkewnesses,
                                             reportSlope, reportSlopes,
                                             reportSpread, reportSpreads,
                                             reportVariance, reportVariances,
                                             reportZerocrossing, reportZerocrossings,
                                             host = makeHost(),
                                             port = int(*portParameter),
                                             root, bandMagnitudes,
                                             channels = static_cast<unsigned int>(std::min(totalNumInputChannels, 64))]() mutable
        {
            juce::OSCSender sender;
            sender.connect(host, port);

            juce::String mix = "mix/";
            juce::String time = "time/";
            juce::String freq = "freq/";
            juce::String spec = "spec/";

            sender.send(juce::OSCAddressPattern(root + mix + time + "kurtosis"), reportAmpKurtosis);
            sender.send(juce::OSCAddressPattern(root + mix + time + "peak"), reportAmpPeak);
            sender.send(juce::OSCAddressPattern(root + mix + time + "pitch"), reportF0);
            sender.send(juce::OSCAddressPattern(root + mix + time + "rms"), reportRms);
            sender.send(juce::OSCAddressPattern(root + mix + time + "skewness"), reportAmpSkewness);
            sender.send(juce::OSCAddressPattern(root + mix + time + "variance"), reportVariance);
            sender.send(juce::OSCAddressPattern(root + mix + time + "zerocrossing"), reportZerocrossing);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "centroid"), reportCentroid);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "decrease"), reportDecrease);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "entropy"), reportEntropy);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "flatness"), reportFlatness);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "flux"), reportFlux);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "irregularity"), reportIrregularity);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "kurtosis"), reportKurtosis);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "peak"), reportPeak);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "rolloff"), reportRolloff);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "scf"), reportScf);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "skewness"), reportSkewness);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "slope"), reportSlope);
            sender.send(juce::OSCAddressPattern(root + mix + freq + "spread"), reportSpread);
            for (auto b = 0u; b < bandMagnitudes.at(64).size(); b++)
            {
                std::string b_str = "band";
                if (b < 9)
                {
                    b_str += "0";
                }
                b_str += std::to_string(b + 1u);
                sender.send(juce::OSCAddressPattern(root + mix + spec + b_str), bandMagnitudes.at(64).at(b));
            }

            for (auto ch = 0u; ch < channels; ch++)
            {
                std::string ch_str = "ch";
                if (ch < 9)
                {
                    ch_str += "0";
                }
                ch_str += std::to_string(ch + 1u) + "/";

                sender.send(juce::OSCAddressPattern(root + ch_str + time + "kurtosis"), reportAmpKurtoses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "peak"), reportAmpPeaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "pitch"), reportF0s.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "rms"), reportChRms.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "skewness"), reportAmpSkewnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "variance"), reportVariances.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + time + "zerocrossing"), reportZerocrossings.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "centroid"), reportCentroids.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "decrease"), reportDecreases.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "entropy"), reportEntropies.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "flatness"), reportFlatnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "flux"), reportFluxes.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "irregularity"), reportIrregularities.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "kurtosis"), reportKurtoses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "peak"), reportPeaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "rolloff"), reportRolloffs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "scf"), reportScfs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "skewness"), reportSkewnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "slope"), reportSlopes.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + freq + "spread"), reportSpreads.at(ch));
                for (auto b = 0u; b < bandMagnitudes.at(ch).size(); b++)
                {
                    std::string b_str = "band";
                    if (b < 9u)
                    {
                        b_str += "0";
                    }
                    b_str += std::to_string(b + 1u);
                    sender.send(juce::OSCAddressPattern(root + ch_str + spec + b_str), bandMagnitudes.at(ch).at(b));
                }
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
