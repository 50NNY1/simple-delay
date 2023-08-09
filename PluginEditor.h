#pragma once
#include "PluginProcessor.h"
//==============================================================================
class otherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    otherLookAndFeel()
    {
        // general slider colours
        setColour(juce::Slider::thumbColourId, juce::Colours::fuchsia);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::lightyellow);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::lightslategrey);
        // rotary slider colours
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::lightslategrey);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightyellow);
        // linear slider colours
        setColour(juce::Slider::trackColourId, juce::Colours::lightyellow);
        setColour(juce::Slider::backgroundColourId, juce::Colours::lightslategrey);
    }
    juce::Colour backgroundColour = juce::Colours::lightslategrey;
};
//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    AudioPluginAudioProcessor &processorRef;
    juce::Slider gainSlider, feedbackSlider, mixSlider, timeSlider;
    juce::Label gainLabel, feedbackLabel, mixLabel, timeLabel;
    std::unique_ptr<juce::SliderParameterAttachment>
        gainSliderAttachment,
        feedbackSliderAttachment,
        mixSliderAttachment,
        timeSliderAttachment;
    otherLookAndFeel laf4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
