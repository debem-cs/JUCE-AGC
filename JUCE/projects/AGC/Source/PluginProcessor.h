/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
*/
class AGCAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AGCAudioProcessor();
    ~AGCAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AGCAudioProcessor)

    std::vector<double> gain;                       // One gain per channel
    std::vector<std::vector<float>> levelBuffers;   // One buffer per channel
    std::vector<double> bufferSums;                 // One sum per channel
    std::vector<int> bufferIndices;                 // One index per channel

    // PARAMS
    const double fs = 48000.0;                      
    const double stepSize = 0.1;
    const double maxGain = 10.0;
    const double toHigh = -0.0;
    const double toLow = toHigh - 50.0;
    const double silence = -50.0;

    int winLength = 0;
};
