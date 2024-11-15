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

TheInformerAudioProcessorEditor::TheInformerAudioProcessorEditor(TheInformerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(500, 500);
    setResizeLimits(500, 500, 2000, 2000);
    setResizable(true, p.wrapperType != TheInformerAudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(1.0f);

    // portSlider.setLookAndFeel(&customLook);
    portSlider.setSliderStyle(juce::Slider::IncDecButtons);
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 100, 20);
    portSlider.setPopupDisplayEnabled(false, false, this);

    addAndMakeVisible(&portSlider);
    portAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "port", portSlider);

    title.setText("theINFORMER", juce::dontSendNotification);
    addAndMakeVisible(title);
}

TheInformerAudioProcessorEditor::~TheInformerAudioProcessorEditor()
{
}

void TheInformerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void TheInformerAudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil(getWidth() / 10.0f);

    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 2, blockUI);
    portSlider.setBounds(blockUI * 2, blockUI * 4, blockUI * 6, blockUI);
    /*
    float dpi = juce::Desktop::getInstance().getDisplays().getMainDisplay().dpi;
    float fontPoints = juce::jmax(1.0f, 2.0f * blockUI * 72.0f / dpi);
    customFont.setHeight(fontPoints);
    title.setFont(customFont);
    */
    title.setJustificationType(juce::Justification::centred);
    title.setBounds(blockUI, blockUI, blockUI * 9, blockUI);
}
