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

    customTypeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::Font_ttf, BinaryData::Font_ttfSize);
    customFont = juce::Font(customTypeface);
    
    getLookAndFeel().setColour(juce::TextEditor::textColourId, juce::Colours::darkslategrey);
    getLookAndFeel().setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::darkslategrey);
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setLookAndFeel(&customLookAndFeel);
        ipSliders[i].setColour(juce::Slider::textBoxTextColourId, juce::Colours::darkslategrey);
        ipSliders[i].setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
        ipSliders[i].setSliderStyle(juce::Slider::LinearBar);
        ipSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
        ipSliders[i].setPopupDisplayEnabled(false, false, this);

        addAndMakeVisible(&(ipSliders[i]));
        ipAttachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "ip" + std::to_string(i+1), ipSliders[i]);
    }

    hostLabel.setText("host", juce::dontSendNotification);
    addAndMakeVisible(hostLabel);

    portSlider.setLookAndFeel(&customLookAndFeel);
    portSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::darkslategrey);
    portSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    portSlider.setSliderStyle(juce::Slider::LinearBar);
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    portSlider.setPopupDisplayEnabled(false, false, this);

    addAndMakeVisible(&portSlider);
    portAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "port", portSlider);

    portLabel.setText("port", juce::dontSendNotification);
    addAndMakeVisible(portLabel);

    title.setText("the informer", juce::dontSendNotification);
    addAndMakeVisible(title);

    rootLabel.setText("root", juce::dontSendNotification);
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
    rootEditor.setTextToShowWhenEmpty("ENTER OSC ADDRESS ROOT", juce::Colours::grey);
    //addressEditor.setFont(juce::Font(14.0f));
    addAndMakeVisible(rootEditor);
}

TheInformerAudioProcessorEditor::~TheInformerAudioProcessorEditor()
{
}

void TheInformerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::whitesmoke);
}

void TheInformerAudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil(getWidth() / 20.0f);
    fontSize = (float)blockUI * 0.8f;
    customFont = customFont.withHeight(fontSize);

    portLabel.setJustificationType(juce::Justification::right);
    portLabel.setBounds(blockUI * 2, blockUI * 5, blockUI * 5, blockUI);
    portLabel.setFont(customFont.withHeight(fontSize));
    portSlider.setBounds(blockUI * 8, blockUI * 5, blockUI * 8, blockUI);
    
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 8, blockUI);
    
    hostLabel.setJustificationType(juce::Justification::right);
    hostLabel.setBounds(blockUI * 2, blockUI * 4, blockUI * 5, blockUI);
    hostLabel.setFont(customFont.withHeight(fontSize));
    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setBounds(blockUI * (8 + (int)(i*2)), blockUI * 4, blockUI * 2, blockUI);
        ipSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 2, blockUI);
    }
    
    rootLabel.setJustificationType(juce::Justification::right);
    rootLabel.setBounds(blockUI * 2, blockUI * 6, blockUI * 5, blockUI);
    rootLabel.setFont(customFont.withHeight(fontSize));
    rootEditor.setBounds(blockUI * 8, blockUI * 6, blockUI * 8, blockUI);
    rootEditor.setFont(customFont.withHeight(fontSize));
    /*
    float dpi = juce::Desktop::getInstance().getDisplays().getMainDisplay().dpi;
    float fontPoints = juce::jmax(1.0f, 2.0f * blockUI * 72.0f / dpi);
    customFont.setHeight(fontPoints);
    title.setFont(customFont);
    */
    title.setJustificationType(juce::Justification::centred);
    title.setBounds(blockUI * 2, blockUI, blockUI * 18, blockUI * 2);
    title.setFont(customFont.withHeight(fontSize * 2.0f));
}
