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
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      customTypeface(juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize)),
      customFont(juce::Font(juce::FontOptions().withTypeface(customTypeface)))
{
    setSize(500, 200);
    setResizeLimits(500, 200, 3000, 1200);
    setResizable(true, p.wrapperType != TheInformerAudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(2.5f);

    std::unique_ptr<juce::XmlElement> svgXml(juce::XmlDocument::parse(BinaryData::Logo_svg));
    if (svgXml != nullptr)
    {
        logo = juce::Drawable::createFromSVG(*svgXml);
    }

    getLookAndFeel().setColour(juce::TextEditor::textColourId, juce::Colours::darkslategrey);
    getLookAndFeel().setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
    getLookAndFeel().setColour(juce::Label::backgroundColourId, juce::Colours::darkslategrey);
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colours::darkslategrey);
    getLookAndFeel().setDefaultSansSerifTypeface(customTypeface);

    title.setText("the informer", juce::dontSendNotification);
    addAndMakeVisible(title);

    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setLookAndFeel(&customLookAndFeel);
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
    portSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    portSlider.setSliderStyle(juce::Slider::LinearBar);
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    portSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(&portSlider);
    portAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "port", portSlider);

    portLabel.setJustificationType(juce::Justification::centred);
    portLabel.setText("port", juce::dontSendNotification);
    addAndMakeVisible(portLabel);

    rootEditor.setInputRestrictions(64, allowedChars);
    rootEditor.setText(audioProcessor.rootValue.toString(), juce::dontSendNotification);
    rootEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::darkslategrey);

    rootEditor.onTextChange = [this]()
    {
        if (rootEditor.getText() != "")
        {
            audioProcessor.rootValue = rootEditor.getText();
        }
    };
    addAndMakeVisible(rootEditor);

    rootLabel.setText("root", juce::dontSendNotification);
    addAndMakeVisible(rootLabel);
}

TheInformerAudioProcessorEditor::~TheInformerAudioProcessorEditor()
{
}

void TheInformerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::whitesmoke);

    if (logo != nullptr)
    {
        logo->drawWithin(g, logoBounds, juce::RectanglePlacement::xRight | juce::RectanglePlacement::yBottom, 1.0f);
    }
}

void TheInformerAudioProcessorEditor::resized()
{
    const int blockUI = (int)ceil(getWidth() / 20.0f);
    fontSize = (float)blockUI * 0.75f;
    customFont = customFont.withHeight(fontSize);

    title.setJustificationType(juce::Justification::centred);
    title.setBounds(blockUI, blockUI, blockUI * 12, blockUI * 2);
    title.setFont(customFont.withHeight(fontSize * 2.0f));

    logoBounds = juce::Rectangle<float>(0.0f, (float)blockUI / 1.25f, (float)getWidth(), (float)getHeight() - ((float)blockUI / 1.25f));

    hostLabel.setJustificationType(juce::Justification::centred);
    hostLabel.setBounds(blockUI, blockUI * 4, blockUI * 3, blockUI);
    hostLabel.setFont(customFont.withHeight(fontSize));
    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setBounds(blockUI * (5 + (int)(i*2)), blockUI * 4, blockUI * 2, blockUI);
        ipSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 2, blockUI);
    }

    portLabel.setBounds(blockUI, blockUI * 5, blockUI * 3, blockUI);
    portLabel.setFont(customFont.withHeight(fontSize));
    portSlider.setBounds(blockUI * 5, blockUI * 5, blockUI * 8, blockUI);
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 8, blockUI);

    rootLabel.setJustificationType(juce::Justification::centred);
    rootLabel.setBounds(blockUI, blockUI * 6, blockUI * 3, blockUI);
    rootLabel.setFont(customFont.withHeight(fontSize));
    rootEditor.setBounds(blockUI * 5, blockUI * 6, blockUI * 8, blockUI);
    rootEditor.applyFontToAllText(customFont.withHeight(fontSize));
}
