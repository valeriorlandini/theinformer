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
    std::make_unique<juce::AudioParameterInt>("port", "Port", 1, 10000, 9000),
        std::make_unique<juce::AudioParameterInt>("ip1", "IP address Octet 1", 0, 255, 127),
        std::make_unique<juce::AudioParameterInt>("ip2", "IP address Octet 2", 0, 255, 0),
        std::make_unique<juce::AudioParameterInt>("ip3", "IP address Octet 3", 0, 255, 0),
        std::make_unique<juce::AudioParameterInt>("ip4", "IP address Octet 4", 0, 255, 1),
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
    index = 0;

    if (index)
    {
        // Dummy, to avoid warnings from some compilers
    }
}

const juce::String TheInformerAudioProcessor::getProgramName(int index)
{
    if (index)
    {
        // Dummy, to avoid warnings from some compilers
    }
    return {};
}

void TheInformerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    if (index)
    {
        // Dummy, to avoid warnings from some compilers
    }
    auto dummy = newName;
}

void TheInformerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (sampleRate > 48000.0f)
    {
        fftSize = (unsigned int)fftSizeLarge;
        fftProcessor = fftProcessorLarge;
        window = windowLarge;
    }
    else
    {
        fftSize = (unsigned int)fftSizeSmall;
        fftProcessor = fftProcessorSmall;
        window = windowSmall;
    }
    updateBlocks = fftSize / samplesPerBlock;
    if (updateBlocks == 0)
    {
        updateBlocks++;
    }

    fftBandwidth = (float)sampleRate / (float)fftSize;
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

    if (midiMessages.isEmpty())
    {
        // Dummy, to avoid warnings from some compilers
    }

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

        float scf = 0.0f;
        std::vector<float> scfs;

        float spread = 0.0f;
        std::vector<float> spreads;

        float flatness = 0.0f;
        std::vector<float> flatnesses;

        float fpeak = 0.0f;
        std::vector<float> fpeaks;

        float rolloff = 0.0f;
        std::vector<float> rolloffs;

        for (unsigned int ch = 0; ch < (unsigned int)std::min(totalNumInputChannels, 64); ch++)
        {
            /* AMPLITUDE DESCRIPTORS */

            chRms.push_back(std::sqrt(sqrGains[ch] / (buffer.getNumSamples() * (float)counter)));
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
            chVariance /= buffer.getNumSamples() * (float)counter;

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

                chKurtosis /= buffer.getNumSamples() * (float)counter;
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
            fftData.at(ch).resize((unsigned int)fftSize * 2);
            for (unsigned int s = 0; s < (unsigned int)std::min((int)samples.at(ch).size(), fftSize); s++)
            {
                fftData.at(ch).at(s) = samples.at(ch).at(s);
            }
            window.get().multiplyWithWindowingTable(fftData.at(ch).data(), (unsigned int)fftSize);
            fftProcessor.get().performFrequencyOnlyForwardTransform(fftData.at(ch).data());

            float a = 0.0f;
            float chCentroid = 0.0f;
            float chFlatness = 0.0f;
            float chScf = 0.0f;
            float chSpread = 0.0f;
            float cumulPower = 0.0f;
            float maxMagnitude = 0.0f;
            float magnLnSum = 0.0f;
            float magnSum = 0.0f;
            std::vector<float> power;
            float powerSum = 0.0f;
            float chRolloff = 0.0f;
            for (auto k = 0; k < fftSize / 2; k++)
            {
                float magnitude = abs(fftData.at(ch).at((unsigned int)k));
                power.push_back(magnitude * magnitude);
                powerSum += power.at((unsigned int)k);
                if (magnitude > maxMagnitude)
                {
                    maxMagnitude = magnitude;
                }
                float frequency = k * fftBandwidth;

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
            }
            if (abs(magnSum) > 0.00001f)
            {
                chCentroid = a / magnSum;
                chScf = maxMagnitude / magnSum;

                float magnMean = magnSum / (fftSize / 2);
                float magnGeoMean = std::exp(magnLnSum / (fftSize / 2));
                chFlatness = magnGeoMean / magnMean;
            }

            float rolloffThresh = powerSum * 0.85f;
            bool threshReached = false;
            a = 0.0f;
            for (auto k = 0; k < fftSize / 2; k++)
            {
                float frequency = k * fftBandwidth;
                a += power.at((unsigned int)k) * std::powf(frequency - chCentroid, 2.0);
                
                if (cumulPower < rolloffThresh)
                {
                    cumulPower += power.at((unsigned int)k);
                }
                if (cumulPower >= rolloffThresh && !threshReached)
                {
                    threshReached = true;
                    chRolloff = k * fftBandwidth;
                }
            }
            if (abs(powerSum) > 0.00001f)
            {
                chSpread = std::sqrtf(a / powerSum);
            }

            auto fpeakBand = std::max_element(fftData.at(ch).begin(), fftData.at(ch).end());
            float chFpeak = (float)std::distance(fftData.at(ch).begin(), fpeakBand) * fftBandwidth;

            centroids.push_back(chCentroid);
            centroid += chCentroid;
            flatnesses.push_back(chFlatness);
            flatness += chFlatness;
            fpeaks.push_back(chFpeak);
            fpeak += chFpeak;
            rolloffs.push_back(chRolloff);
            rolloff += chRolloff;
            scfs.push_back(chScf);
            scf += chScf;
            spreads.push_back(chSpread);
            spread += chSpread;
        }

        centroid /= totalNumInputChannels;
        flatness /= totalNumInputChannels;
        fpeak /= totalNumInputChannels;
        kurtosis /= totalNumInputChannels;
        rolloff /= totalNumInputChannels;
        rms /= totalNumInputChannels;
        scf /= totalNumInputChannels;
        spread /= totalNumInputChannels;
        variance /= totalNumInputChannels;

        for (unsigned int i = 0; i < 64; i++)
        {
            samples.at(i).clear();
        }

        juce::String root = "/" + rootValue.toString() + "/";

        std::function<void()> reportStats = [centroid, centroids,
                                             flatness, flatnesses,
                                             fpeak, fpeaks,
                                             kurtosis, kurtoses,
                                             peak, peaks,
                                             rms, chRms,
                                             rolloff, rolloffs,
                                             scf, scfs,
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
            sender.send(juce::OSCAddressPattern(root + mix + "flatness"), flatness);
            sender.send(juce::OSCAddressPattern(root + mix + "freqpeak"), fpeak);
            sender.send(juce::OSCAddressPattern(root + mix + "rolloff"), rolloff);
            sender.send(juce::OSCAddressPattern(root + mix + "scf"), scf);
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
                sender.send(juce::OSCAddressPattern(root + ch_str + "flatness"), flatnesses.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "freqpeak"), fpeaks.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "rolloff"), rolloffs.at(ch));
                sender.send(juce::OSCAddressPattern(root + ch_str + "scf"), scfs.at(ch));
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
