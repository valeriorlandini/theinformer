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

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_osc/juce_osc.h>
#include <cmath>
#include "BinaryData.h"

class TheInformerAudioProcessor : public juce::AudioProcessor
{
public:
    TheInformerAudioProcessor();
    ~TheInformerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setHost(const juce::String& newHost);
    bool connect();

    juce::AudioProcessorValueTreeState treeState;

    std::array<std::atomic<float>*, 4> ipParameters = {nullptr, nullptr, nullptr, nullptr};
    juce::String host = "127.0.0.1";

    std::atomic<float>* portParameter = nullptr;

    std::atomic<juce::String>* hostParameter = nullptr;

    juce::Value rootValue;

private:
    juce::OSCSender sender;
    int updateBlocks = 1;
    int counter = 0;
    float updateInterval = 0.05f;
    std::array<float, 64> sqrGains = { 0.0f };
    std::array<std::vector<float>, 64> samples;

    // 2048 samples, for sample rates up to 48000 Hz
    static constexpr int orderSmall = 11;
    static constexpr int fftSizeSmall = 1 << orderSmall;
    juce::dsp::FFT fftProcessorSmall;
    juce::dsp::WindowingFunction<float> windowSmall;

    // 4096 samples, for sample rates over 48000 Hz
    static constexpr int orderLarge = 12;
    static constexpr int fftSizeLarge = 1 << orderLarge;
    juce::dsp::FFT fftProcessorLarge;
    juce::dsp::WindowingFunction<float> windowLarge;

    float fftBandwidth = 44100.0f / (float)fftSizeSmall;
    int fftSize = fftSizeSmall;

    std::reference_wrapper<juce::dsp::FFT> fftProcessor = fftProcessorSmall;
    std::reference_wrapper<juce::dsp::WindowingFunction<float>> window = windowSmall;
    std::array<std::vector<float>, 64> fftData;

    int port = 9000;

    inline juce::String makeHost()
    {
        juce::String hostString = "";

        for (unsigned int i = 0; i < 4; i++)
        {
            hostString += juce::String(int(*(ipParameters[i])));
            if (i < 3)
            {
                hostString += ".";
            }
        }

        return hostString;
    }

    static std::atomic<int> instanceCounter;
    int instance;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheInformerAudioProcessor)
};
