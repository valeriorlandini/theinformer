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

#include "PluginProcessor.h"

class TheInformerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    TheInformerAudioProcessorEditor(TheInformerAudioProcessor&);
    ~TheInformerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TheInformerAudioProcessor& audioProcessor;
    juce::Label title;
    juce::Label addressLabel;
    juce::Label portLabel;
    juce::Slider portSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> portAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheInformerAudioProcessorEditor)
};
