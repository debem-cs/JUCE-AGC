/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AGCAudioProcessor::AGCAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
#else
    : parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AGCAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "REF", "Reference Level (dB)", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 75.0f));

    return { params.begin(), params.end() };
}

AGCAudioProcessor::~AGCAudioProcessor()
{
}

//==============================================================================
const juce::String AGCAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AGCAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AGCAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AGCAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AGCAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AGCAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AGCAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AGCAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AGCAudioProcessor::getProgramName (int index)
{
    return {};
}

void AGCAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AGCAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    winLength = static_cast<int>(0.4 * sampleRate);

    // Get the actual number of input channels
    int numChannels = getTotalNumInputChannels();

    // Resize all channel-specific containers
    gain.resize(numChannels, 0.0);
    levelBuffers.resize(numChannels);
    bufferSums.resize(numChannels, 0.0);
    bufferIndices.resize(numChannels, 0);

    // Initialize each channel's buffer
    for (auto& buffer : levelBuffers) {
        buffer.assign(winLength, 1.0f);
    }

    // Initialize each channel's sum
    for (auto& sum : bufferSums) {
        sum = static_cast<double>(winLength);
    }
}

void AGCAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AGCAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AGCAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels > gain.size()) {
        prepareToPlay(getSampleRate(), buffer.getNumSamples());
    }

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);

        for (int n = 0; n < numSamples; ++n)
        {
            float x = channelData[n];

            if (std::abs(x) > std::pow(10.0f, silence / 20.0f))
            {
                bufferSums[ch] = bufferSums[ch] - levelBuffers[ch][bufferIndices[ch]] + (x * x);
                levelBuffers[ch][bufferIndices[ch]] = x * x;
                bufferIndices[ch] = (bufferIndices[ch] + 1) % winLength;
            }

            float y = x * std::exp(gain[ch]);
            y = std::clamp(y, -1.0f, 1.0f);

            channelData[n] = y;

            double pow = bufferSums[ch] / winLength;
            pow = std::max(pow, std::numeric_limits<double>::epsilon());

            // remap 0 a 100 para -66 a -26
            // toLow + (toHigh - toLow) * ((value - fromLow) / (fromHigh - fromLow))

            float ref = toLow + (toHigh - toLow) * ((parameters.getRawParameterValue("REF")->load() - 0.0) / (100.0 - 0.0));
            double e = std::log(std::pow(10.0, ref / 10.0)) - (std::log(pow) + 2.0 * gain[ch]);

            gain[ch] += stepSize * e;
            gain[ch] = std::min(gain[ch], maxGain);
        }
    }
}

//==============================================================================
bool AGCAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AGCAudioProcessor::createEditor()
{
    return new AGCAudioProcessorEditor (*this);
}

//==============================================================================
void AGCAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AGCAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AGCAudioProcessor();
}
