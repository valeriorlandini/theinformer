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
    
    std::atomic<float>* normParameter = nullptr;

    std::atomic<float>* reportBandsParameter = nullptr;

    juce::Value rootValue;

private:
    unsigned int updateBlocks = 1u;
    unsigned int counter = 0u;
    unsigned int expectedSamples = 0u;
    float updateInterval = 0.05f;
    std::array<float, 64> sqrGains = { 0.0f };
    std::array<std::vector<float>, 64> samples;
    std::array<std::vector<float>, 64> nextSamples;
    std::vector<float> bandsEdges = {0.0f, 386.196f, 2485.79f, 22050.0f};
    std::vector<float> binsPerReportedBand;

    float invNyquist = 1.0f / 44100.0f;

    // 4096 samples, for sample rates up to 48000 Hz
    static constexpr int orderSmall = 12;
    static constexpr int fftSizeSmall = 1 << orderSmall;
    juce::dsp::FFT fftProcessorSmall;
    juce::dsp::WindowingFunction<float> windowSmall;

    // 8192 samples, for sample rates over 48000 Hz
    static constexpr int orderLarge = 13;
    static constexpr int fftSizeLarge = 1 << orderLarge;
    juce::dsp::FFT fftProcessorLarge;
    juce::dsp::WindowingFunction<float> windowLarge;

    float fftBandwidth = 44100.0f / (float)fftSizeSmall;
    std::array<float, fftSizeLarge / 2> frequencies = {};
    std::array<std::array<float, fftSizeLarge / 2>, 64> magnitudes = {};
    std::array<std::array<float, fftSizeLarge / 2>, 64> prev_magnitudes = {};
    unsigned int fftSize = static_cast<unsigned int>(fftSizeSmall);

    std::reference_wrapper<juce::dsp::FFT> fftProcessor = fftProcessorSmall;
    std::reference_wrapper<juce::dsp::WindowingFunction<float>> window = windowSmall;
    std::array<std::vector<float>, 64> fftData;
    
    int port = 9000;

    unsigned int reportBands = 3u;

    inline juce::String makeHost()
    {
        juce::String hostString = "";

        for (unsigned int i = 0; i < 4; i++)
        {
            hostString += juce::String(static_cast<int>((*(ipParameters[i]))));
            if (i < 3)
            {
                hostString += ".";
            }
        }

        return hostString;
    }

    inline void getEqualOctaveBandEdges() 
    {   
        if (reportBands > 1u)
        {
            const float f_min = 60.0f;
            const float f_max = 16000.0f;
            const float nyquist = 1.0f / invNyquist;
        
            float totalOctaves = std::log2f(f_max / f_min);
            float octavesPerBand = totalOctaves / static_cast<float>(reportBands);
        
            bandsEdges.resize(reportBands + 1u);
        
            bandsEdges[0] = 0.0f;
            for (auto i = 0u; i < reportBands; ++i)
            {
                bandsEdges[i + 1u] = f_min * std::powf(2.0f, i * octavesPerBand);
            }
            bandsEdges[reportBands] = nyquist;

            computeBinsPerReportedBand();
        }
    }

    inline void computeBinsPerReportedBand()
    {
        if (reportBands > 1u)
        {
            binsPerReportedBand.resize(reportBands);
    
            for (auto band = 0u; band < reportBands; ++band)
            {
                float bandStartFreq = bandsEdges[band];
                float bandEndFreq = bandsEdges[band + 1u];
        
                unsigned int binStart = static_cast<unsigned int>(std::floor(bandStartFreq / fftBandwidth));
                unsigned int binEnd = static_cast<unsigned int>(std::ceil(bandEndFreq / fftBandwidth));
        
                binStart = std::max(0u, binStart);
                binEnd = std::min(fftSize / 2u, binEnd);
            
                binsPerReportedBand[band] = static_cast<float>(std::max(0u, binEnd - binStart));
            }
        }
    }

    static std::atomic<int> instanceCounter;
    int instance;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheInformerAudioProcessor)
};
