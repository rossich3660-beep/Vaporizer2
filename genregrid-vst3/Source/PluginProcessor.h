#pragma once
#include <JuceHeader.h>
#include <vector>

class GenreGridMidiAudioProcessor : public juce::AudioProcessor
{
public:
    struct MidiEvent { double beat = 0.0; juce::MidiMessage message; };
    GenreGridMidiAudioProcessor();
    ~GenreGridMidiAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override { return true; }
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "GenreGrid MIDI"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    bool loadMidiFile(const juce::File& file);
    juce::String getLoadedFileName() const;
    void setVariation(float value) { variation.store(juce::jlimit(0.0f, 1.0f, value)); }
    float getVariation() const { return variation.load(); }
    bool hasLoadedMidi() const { return !events.empty(); }
    double getPhraseLengthBeats() const { return phraseLengthBeats; }

private:
    std::vector<MidiEvent> events;
    double phraseLengthBeats = 4.0;
    double sampleRateHz = 44100.0;
    double fallbackPpq = 0.0;
    juce::String loadedFileName = "No MIDI loaded";
    std::atomic<float> variation { 0.35f };
    juce::CriticalSection midiLock;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenreGridMidiAudioProcessor)
};
