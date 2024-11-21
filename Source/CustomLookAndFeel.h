#pragma once

#include "PluginProcessor.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    juce::Font customFont;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};
