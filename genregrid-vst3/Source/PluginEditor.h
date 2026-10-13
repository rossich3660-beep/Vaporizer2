#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class GenreGridMidiAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit GenreGridMidiAudioProcessorEditor(GenreGridMidiAudioProcessor&);
    ~GenreGridMidiAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    GenreGridMidiAudioProcessor& processor;
    juce::Label title;
    juce::Label status;
    juce::TextButton loadButton { "LOAD MIDI" };
    juce::Label variationLabel;
    juce::Slider variationSlider;
    juce::Label info;
    std::unique_ptr<juce::FileChooser> chooser;
    void chooseMidi();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenreGridMidiAudioProcessorEditor)
};
