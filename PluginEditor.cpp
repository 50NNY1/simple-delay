#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    timeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    gainLabel.setText("Gain", juce::dontSendNotification);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    mixLabel.setText("Mix", juce::dontSendNotification);
    timeLabel.setText("Time", juce::dontSendNotification);

    std::vector<std::pair<juce::Slider *, juce::Label *>> sliderLabelPairs = {
        {&gainSlider, &gainLabel},
        {&feedbackSlider, &feedbackLabel},
        {&mixSlider, &mixLabel},
        {&timeSlider, &timeLabel}};

    for (const auto &pair : sliderLabelPairs)
    {
        juce::Slider *slider = pair.first;
        juce::Label *label = pair.second;

        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 200, 30);
        slider->setLookAndFeel(&laf4);
        addAndMakeVisible(slider);

        label->attachToComponent(slider, false);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }
    auto &apvts = processorRef.state;
    gainSliderAttachment = std::make_unique<juce::SliderParameterAttachment>(*apvts.getParameter("GAIN"), gainSlider);
    feedbackSliderAttachment = std::make_unique<juce::SliderParameterAttachment>(*apvts.getParameter("FEEDBACK"), feedbackSlider);
    mixSliderAttachment = std::make_unique<juce::SliderParameterAttachment>(*apvts.getParameter("MIX"), mixSlider);
    timeSliderAttachment = std::make_unique<juce::SliderParameterAttachment>(*apvts.getParameter("TIME"), timeSlider);
    setSize(400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(laf4.backgroundColour);
}

void AudioPluginAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    int margin = 20;

    juce::Rectangle<int> gainBounds = bounds.removeFromRight(getWidth() / 3);
    gainSlider.setBounds(gainBounds.reduced(margin));

    juce::Rectangle<int> knobsBounds = bounds.removeFromTop(getHeight() / 2);
    juce::Rectangle<int> feedbackBounds = knobsBounds.removeFromLeft(knobsBounds.getWidth() / 2);
    feedbackSlider.setBounds(feedbackBounds.reduced(margin));
    mixSlider.setBounds(knobsBounds.reduced(margin));

    juce::Rectangle<int> timeBounds = bounds.removeFromLeft(bounds.getWidth() / 2);
    timeSlider.setBounds(timeBounds.reduced(margin));
}