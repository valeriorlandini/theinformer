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

TheInformerAudioProcessor::TheInformerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : treeState(*this, nullptr, juce::Identifier("InformerParameters"),
        {
            std::make_unique<juce::AudioParameterInt>("port", "Port", 1, 10000, 9000),
            std::make_unique<juce::AudioParameterInt>("bit2Status", "Bit 2", 0, 3, 0),
            std::make_unique<juce::AudioParameterInt>("bit3Status", "Bit 3", 0, 3, 0),
            std::make_unique<juce::AudioParameterInt>("bit4Status", "Bit 4", 0, 3, 0),
            std::make_unique<juce::AudioParameterInt>("bit5Status", "Bit 5", 0, 3, 0),
            std::make_unique<juce::AudioParameterInt>("bit6Status", "Bit 6", 0, 3, 0),
            std::make_unique<juce::AudioParameterInt>("bit7Status", "Bit 7", 0, 3, 0),
            std::make_unique<juce::AudioParameterInt>("bit8Status", "Bit 8", 0, 3, 0),
            std::make_unique<juce::AudioParameterBool>("randomize", "Randomize Bits", false),
            std::make_unique<juce::AudioParameterBool>("dcfilter", "DC Filter", true),
            std::make_unique<juce::AudioParameterFloat>("lowcut", "Master Lowpass", juce::NormalisableRange<float>(20.0f, 20000.0f), 20000.0f),
            std::make_unique<juce::AudioParameterFloat>("wet", "Wet", juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f)
        }),
    AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    fftProcessor(order),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    portParameter = treeState.getRawParameterValue("port");
    port = int(*portParameter);
    sender.connect("127.0.0.1", port);
}

TheInformerAudioProcessor::~TheInformerAudioProcessor()
{
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
}

const juce::String TheInformerAudioProcessor::getProgramName(int index)
{
    index = 0;
    return {};
}

void TheInformerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    index = 0;
    auto dummy = newName;
}

void TheInformerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    updateBlocks = fftSize / samplesPerBlock;
    if (updateBlocks == 0)
    {
        updateBlocks++;
    }

    fftBandwidth = (float)sampleRate / (float)fftSize;
}

void TheInformerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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
        // Dummy, to avoid warning from some compilers
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (auto i = 0; i < buffer.getNumSamples(); i++)
        {
            if (channel < 64)
            {
                sqrGains[channel] += channelData[i] * channelData[i];
                samples.at(channel).push_back(channelData[i]);
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

        float bandwidth = 0.0f;
        std::vector<float> bandwidths;

        std::vector<float> fpeaks;

        for (auto ch = 0; ch < std::min(totalNumInputChannels, 64); ch++)
        {
            for (auto s = 0; s < std::min((int)samples.at(ch).size(), fftSize); s++)
            {
                fftData.at(ch).at(s) = samples.at(ch).at(s);
            }
            window.multiplyWithWindowingTable(fftData.at(ch).data(), fftSize);
            fftProcessor.performFrequencyOnlyForwardTransform(fftData.at(ch).data());

            float chCentroid = 0.0f;
            float chScf = 0.0f;
            float chFlatness = 0.0f;
            float chBandwidth = 0.0f;
            float a = 0.0f;
            float magnSum = 0.0f;
            float magnLnSum = 0.0f;
            float maxMagnitude = 0.0f;
            for (auto k = 0; k < fftSize / 2; k++)
            {
                float magnitude = abs(fftData.at(ch).at(k));
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
            if (magnSum != 0.0f)
            {
                chCentroid = a / magnSum;
                chScf = maxMagnitude / magnSum;

                float magnMean = magnSum / (fftSize / 2);
                float magnGeoMean = std::exp(magnLnSum / (fftSize / 2));
                chFlatness = magnGeoMean / magnMean;

                float bwSum = 0.0f;
                for (auto k = 0; k < fftSize / 2; k++)
                {
                    float magnitude = abs(fftData.at(ch).at(k));
                    float frequency = k * fftBandwidth;
                    bwSum += magnitude * std::powf((frequency - chCentroid), 2.0f);
                }

                chBandwidth = std::powf(bwSum, 0.5f);
            }

            centroids.push_back(chCentroid);
            scfs.push_back(chScf);
            flatnesses.push_back(chFlatness);
            bandwidths.push_back(chBandwidth);
            centroid += chCentroid;
            bandwidth += chBandwidth;
            scf += chScf;
            flatness += chFlatness;

            float chSpread = 0.0f;
            float powerSum = 0.0f;
            a = 0.0f;
            for (auto k = 0; k < fftSize / 2; k++)
            {
                float power = std::powf(fftData.at(ch).at(k), 2.0f);

                float frequency = k * fftBandwidth;
                a += power * std::powf(frequency - chCentroid, 2.0);
                powerSum += power;
            }
            if (powerSum != 0.0f)
            {
                chSpread = std::sqrtf(a / powerSum);
            }
            spreads.push_back(chSpread);
            spread += chSpread;

            auto fpeak = std::max_element(fftData.at(ch).begin(), fftData.at(ch).end());
            fpeaks.push_back((float)std::distance(fftData.at(ch).begin(), fpeak) * fftBandwidth);
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
            peaks.push_back(chPeak);
            mean /= samples.at(ch).size();
            float chVariance = 0.0f;
            float chKurtosis = 0.0f;
            for (auto const& s : samples.at(ch))
            {
                chVariance += powf(s - mean, 2.0f);
            }
            chVariance /= buffer.getNumSamples() * (float)counter;
            variances.push_back(chVariance);
            variance += chVariance;

            float invSqrChVariance = chVariance * chVariance;

            // If variance is 0, invSqrVariance is left to 0 so that kurtosis and skewness are not undefined
            if (invSqrChVariance != 0.0f)
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
        }

        rms /= totalNumInputChannels;
        variance /= totalNumInputChannels;
        kurtosis /= totalNumInputChannels;
        centroid /= totalNumInputChannels;
        scf /= totalNumInputChannels;
        spread /= totalNumInputChannels;
        flatness /= totalNumInputChannels;
        bandwidth /= totalNumInputChannels;

        for (auto i = 0; i < 64; i++)
        {
            samples.at(i).clear();
        }

        std::function<void()> reportStats = [this, centroid, centroids, scf, scfs,
            flatness, flatnesses, spread, spreads, bandwidth, bandwidths, peak, peaks, rms, chRms,
            variance, variances, kurtosis, kurtoses]() mutable
            {
                if (int(*portParameter) != port)
                {
                    port = int(*portParameter);
                    sender.disconnect();
                    sender.connect("127.0.0.1", port);
                }
                sender.send(juce::OSCAddressPattern(address + "rms"), rms);
                sender.send(juce::OSCAddressPattern(address + "variance"), variance);
                sender.send(juce::OSCAddressPattern(address + "kurtosis"), kurtosis);
                sender.send(juce::OSCAddressPattern(address + "centroid"), centroid);
                sender.send(juce::OSCAddressPattern(address + "scf"), scf);
                sender.send(juce::OSCAddressPattern(address + "flatness"), flatness);
                sender.send(juce::OSCAddressPattern(address + "bandwidth"), bandwidth);
                sender.send(juce::OSCAddressPattern(address + "spread"), spread);
                for (auto ch = 0; ch < chRms.size(); ch++)
                {
                    std::string ch_str = "ch";
                    if (ch < 9)
                    {
                        ch_str += "0";
                    }
                    ch_str += std::to_string(ch + 1) + "/";

                    sender.send(juce::OSCAddressPattern(address + ch_str + "centroid"), centroids.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "scf"), scfs.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "flatness"), flatnesses.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "bandwidth"), bandwidths.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "spread"), spreads.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "peak"), peaks.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "rms"), chRms.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "var"), variances.at(ch));
                    sender.send(juce::OSCAddressPattern(address + ch_str + "kur"), kurtoses.at(ch));
                }
            };
        juce::MessageManager::callAsync(reportStats);

        counter = 0;
    }
}

bool TheInformerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
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
            //addressValue = treeState.state.getPropertyAsValue("address", nullptr);
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheInformerAudioProcessor();
}