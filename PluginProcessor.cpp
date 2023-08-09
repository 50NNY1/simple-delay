#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      state(*this, nullptr, "Parameters", createParameters())

{
    state.addParameterListener("TIME", this);
    state.addParameterListener("FEEDBACK", this);
    state.addParameterListener("MIX", this);
    state.addParameterListener("GAIN", this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    state.removeParameterListener("TIME", this);
    state.removeParameterListener("FEEDBACK", this);
    state.removeParameterListener("MIX", this);
    state.removeParameterListener("GAIN", this);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();

    delay.prepare(spec);
    linear.prepare(spec);
    mixer.prepare(spec);

    for (auto &volume : delayFeedbackVolume)
        volume.reset(spec.sampleRate, 0.05);

    linear.reset();
    std::fill(lastDelayOutput.begin(), lastDelayOutput.end(), 0.0f);
    // int delayMilliseconds = 500;
    // auto delaySamples = (int)std::round(sampleRate * delayMilliseconds / 1000.0);
    // delayBuffer.setSize(getTotalNumOutputChannels(), delaySamples);
    // delayBuffer.clear();
    // delayBufferPos = 0;
    // juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const auto numChannels = juce::jmax(totalNumInputChannels, totalNumOutputChannels);
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, (size_t)numChannels);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);
    const auto &input = context.getInputBlock();
    const auto &output = context.getOutputBlock();
    mixer.pushDrySamples(input);

    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        auto *samplesIn = input.getChannelPointer(channel);
        auto *samplesOut = output.getChannelPointer(channel);

        for (size_t sample = 0; sample < input.getNumSamples(); ++sample)
        {
            auto input = samplesIn[sample] - lastDelayOutput[channel];
            auto delayAmount = delayValue[channel];

            linear.pushSample(int(channel), input);
            linear.setDelay((float)delayAmount);
            samplesOut[sample] = linear.popSample((int)channel);

            lastDelayOutput[channel] = samplesOut[sample] * delayFeedbackVolume[channel].getNextValue();
        }
    }

    mixer.mixWetSamples(output);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    // return new juce::GenericAudioProcessorEditor(*this);
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
    if (auto xmlState = state.copyState().createXml())
        copyXmlToBinary(*xmlState, destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        state.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

void AudioPluginAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID == "RATE")
        std::fill(delayValue.begin(), delayValue.end(), newValue / 1000.0 * getSampleRate());

    if (parameterID == "MIX")
        mixer.setWetMixProportion(newValue);

    if (parameterID == "FEEDBACK")
    {
        const auto feedbackGain = juce::Decibels::decibelsToGain(newValue, -100.0f);

        for (auto &volume : delayFeedbackVolume)
            volume.setTargetValue(feedbackGain);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;

    using Range = juce::NormalisableRange<float>;

    params.add(std::make_unique<juce::AudioParameterFloat>("TIME", "Time", 0.01f, 1000.0f, 0));
    params.add(std::make_unique<juce::AudioParameterFloat>("FEEDBACK", "Feedback", -100.0f, 0.0f, -100.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", Range{0.0f, 1.0f, 0.01f}, 0.0f));
    params.add(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", Range{0.0f, 1.0f, 0.01f}, 1.0f));

    return params;
}
