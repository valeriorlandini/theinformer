/******************************************************************************
This file is part of The Informer.
Copyright 2024-2025 Valerio Orlandini <valeriorlandini@gmail.com>.

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
    setSize(500, 250);
    setResizeLimits(500, 250, 3000, 15000);
    setResizable(true, p.wrapperType != TheInformerAudioProcessor::wrapperType_AudioUnitv3);
    getConstrainer()->setFixedAspectRatio(500.0f/250.0f);

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

    normSlider.setLookAndFeel(&customLookAndFeel);
    normSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    normSlider.setSliderStyle(juce::Slider::LinearBar);
    normSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    normSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(&normSlider);
    normAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "normalize", normSlider);
    normSlider.textFromValueFunction = [](double value)
    {
        return value < 0.5 ? "off" : "on";
    };
    normSlider.updateText();

    normLabel.setJustificationType(juce::Justification::centred);
    normLabel.setText("norm", juce::dontSendNotification);
    addAndMakeVisible(normLabel);

    bandsSlider.setLookAndFeel(&customLookAndFeel);
    bandsSlider.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    bandsSlider.setSliderStyle(juce::Slider::LinearBar);
    bandsSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    bandsSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(&bandsSlider);
    bandsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.treeState, "reportbands", bandsSlider);
    bandsSlider.textFromValueFunction = [](double value)
    {
        return std::to_string(int(value)) + " bands";
    };
    bandsSlider.updateText();

    bandsLabel.setJustificationType(juce::Justification::centred);
    bandsLabel.setText("spec", juce::dontSendNotification);
    addAndMakeVisible(bandsLabel);
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
    const int blockUI = static_cast<int>(ceil(getWidth() / 20.0f));
    fontSize = static_cast<float>(blockUI) * 0.75f;
    customFont = customFont.withHeight(fontSize);

    title.setJustificationType(juce::Justification::centred);
    title.setBounds(blockUI, blockUI, blockUI * 12, blockUI * 2);
    title.setFont(customFont.withHeight(fontSize * 2.0f));

    logoBounds = juce::Rectangle<float>(0.0f, static_cast<float>(blockUI) / 1.25f, static_cast<float>(getWidth()), static_cast<float>(getHeight()) - (static_cast<float>(blockUI) / 1.25f));

    hostLabel.setJustificationType(juce::Justification::centred);
    hostLabel.setBounds(blockUI, blockUI * 4, blockUI * 3, blockUI);
    hostLabel.setFont(customFont.withHeight(fontSize));
    for (unsigned int i = 0; i < 4; i++)
    {
        ipSliders[i].setBounds(blockUI * (5 + static_cast<int>(i*2)), blockUI * 4, blockUI * 2, blockUI);
        ipSliders[i].setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 2, blockUI);
    }

    portLabel.setJustificationType(juce::Justification::centred);
    portLabel.setBounds(blockUI, blockUI * 5, blockUI * 3, blockUI);
    portLabel.setFont(customFont.withHeight(fontSize));
    portSlider.setBounds(blockUI * 5, blockUI * 5, blockUI * 8, blockUI);
    portSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 8, blockUI);

    rootLabel.setJustificationType(juce::Justification::centred);
    rootLabel.setBounds(blockUI, blockUI * 6, blockUI * 3, blockUI);
    rootLabel.setFont(customFont.withHeight(fontSize));
    rootEditor.setBounds(blockUI * 5, blockUI * 6, blockUI * 8, blockUI);
    rootEditor.applyFontToAllText(customFont.withHeight(fontSize));
    rootEditor.setJustification(juce::Justification::centred);
 
    normLabel.setJustificationType(juce::Justification::centred);
    normLabel.setBounds(blockUI, blockUI * 7, blockUI * 3, blockUI);
    normLabel.setFont(customFont.withHeight(fontSize));
    normSlider.setBounds(blockUI * 5, blockUI * 7, blockUI * 8, blockUI);
    normSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 8, blockUI);
 
    bandsLabel.setJustificationType(juce::Justification::centred);
    bandsLabel.setBounds(blockUI, blockUI * 8, blockUI * 3, blockUI);
    bandsLabel.setFont(customFont.withHeight(fontSize));
    bandsSlider.setBounds(blockUI * 5, blockUI * 8, blockUI * 8, blockUI);
    bandsSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, blockUI * 8, blockUI);
}
