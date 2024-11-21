#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
	auto customTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Font_ttf, BinaryData::Font_ttfSize);
    customFont = juce::Font(customTypeface);
}

CustomLookAndFeel::~CustomLookAndFeel()
{
}

juce::Label* CustomLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    // Create a custom label for the slider's text box
    auto* label = new juce::Label();
    label->setJustificationType(juce::Justification::centred);

    float fontSize = slider.getHeight() * 0.75f;
    label->setFont(customFont.withHeight(fontSize));

    label->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label->setColour(juce::Label::outlineColourId, juce::Colours::darkslategrey);


    return label;
}
