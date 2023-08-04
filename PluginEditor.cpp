#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p),
      gainSliderAttachment(p.state, "gain", gainSlider),
      feedbackSliderAttachment(p.state, "feedback", feedbackSlider),
      mixSliderAttachment(p.state, "mix", mixSlider),
      timeSliderAttachment(p.state, "time", timeSlider)
{
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    timeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);

    for (auto *slider : {&gainSlider, &feedbackSlider, &mixSlider, &timeSlider})
    {
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 200, 30);
        addAndMakeVisible(slider);
    }
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black);
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

    juce::Rectangle<int> timeBounds = bounds.removeFromBottom(getHeight() / 2);
    timeSlider.setBounds(timeBounds.reduced(margin));
}
