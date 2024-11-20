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

#include "PluginEditor.h"

TheInformerAudioProcessorEditor::TheInformerAudioProcessorEditor(TheInformerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(500, 250);
    setResizeLimits(500, 250, 3000, 1500);
    setResizable(true, p.wrapperType != TheInformerAudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(2.0f);

    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setSliderStyle(juce::Slider::LinearBar);
        ipSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 100, 20);
        ipSliders[i].setPopupDisplayEnabled(false, false, this);
        ipSliders[i].setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);

        addAndMakeVisible(&(ipSliders[i]));
        ipAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "ip" + std::to_string(i+1), ipSliders[i]);
    }

    hostLabel.setText("Host", juce::dontSendNotification);
    addAndMakeVisible(hostLabel);

    getLookAndFeel().setColour(juce::Slider::trackColourId, juce::Colours::royalblue);
    portSlider.setSliderStyle(juce::Slider::LinearBar);
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 100, 20);
    portSlider.setPopupDisplayEnabled(false, false, this);

    addAndMakeVisible(&portSlider);
    portAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "port", portSlider);

    portLabel.setText("Port", juce::dontSendNotification);
    addAndMakeVisible(portLabel);

    title.setText("The Informer", juce::dontSendNotification);
    addAndMakeVisible(title);

    rootLabel.setText("Root", juce::dontSendNotification);
    addAndMakeVisible(rootLabel);

    rootEditor.setInputRestrictions(64, allowedChars);
    rootEditor.setText(audioProcessor.rootValue.toString(), juce::dontSendNotification);
    rootEditor.onTextChange = [this]()
    {
        if (rootEditor.getText() != "")
        {
            audioProcessor.rootValue = rootEditor.getText();
        }
    };
    rootEditor.setTextToShowWhenEmpty("Enter OSC address root", juce::Colours::grey);
    //addressEditor.setFont(juce::Font(14.0f));
    addAndMakeVisible(rootEditor);
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
    const int blockUI = (int)ceil(getWidth() / 20.0f);

    portLabel.setJustificationType(juce::Justification::right);
    portLabel.setBounds(blockUI * 2, blockUI * 5, blockUI * 5, blockUI);
    //portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 4, blockUI * 2);
    portSlider.setBounds(blockUI * 8, blockUI * 5, blockUI * 8, blockUI);


    hostLabel.setJustificationType(juce::Justification::right);
    hostLabel.setBounds(blockUI * 2, blockUI * 4, blockUI * 5, blockUI);
    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setBounds(blockUI * (8 + (int)(i*2)), blockUI * 4, blockUI * 2, blockUI);
    }
    
    rootLabel.setJustificationType(juce::Justification::right);
    rootLabel.setBounds(blockUI * 2, blockUI * 6, blockUI * 5, blockUI);
    rootEditor.setBounds(blockUI * 8, blockUI * 6, blockUI * 8, blockUI);
    /*
    float dpi = juce::Desktop::getInstance().getDisplays().getMainDisplay().dpi;
    float fontPoints = juce::jmax(1.0f, 2.0f * blockUI * 72.0f / dpi);
    customFont.setHeight(fontPoints);
    title.setFont(customFont);
    */
    title.setJustificationType(juce::Justification::centred);
    title.setBounds(blockUI * 2, blockUI * 2, blockUI * 18, blockUI * 2);
}
