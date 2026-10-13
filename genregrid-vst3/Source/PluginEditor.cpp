#include "PluginEditor.h"

GenreGridMidiAudioProcessorEditor::GenreGridMidiAudioProcessorEditor(GenreGridMidiAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(430, 270);
    title.setText("GENREGRID MIDI", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(23.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colour(0xffe7f3ff));
    title.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(title);

    status.setText(processor.getLoadedFileName(), juce::dontSendNotification);
    status.setColour(juce::Label::textColourId, juce::Colour(0xffb8c7d9));
    status.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(status);

    loadButton.onClick = [this] { chooseMidi(); };
    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff3867f4));
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addAndMakeVisible(loadButton);

    variationLabel.setText("VARIATION", juce::dontSendNotification);
    variationLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe7f3ff));
    addAndMakeVisible(variationLabel);

    variationSlider.setRange(0.0, 1.0, 0.01);
    variationSlider.setValue(processor.getVariation());
    variationSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    variationSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 54, 24);
    variationSlider.onValueChange = [this] { processor.setVariation((float) variationSlider.getValue()); };
    addAndMakeVisible(variationSlider);

    info.setText("Loads a MIDI phrase and outputs looping variations.\nRoute this plugin's MIDI output to an instrument track.", juce::dontSendNotification);
    info.setColour(juce::Label::textColourId, juce::Colour(0xff91a5bd));
    info.setJustificationType(juce::Justification::topLeft);
    info.setFont(juce::Font(juce::FontOptions(12.0f)));
    addAndMakeVisible(info);
}

void GenreGridMidiAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101722));
    g.setColour(juce::Colour(0xff26364a));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 12.0f, 1.0f);
    g.setColour(juce::Colour(0xff5d8cff));
    g.fillRoundedRectangle(18.0f, 18.0f, 4.0f, 36.0f, 2.0f);
}

void GenreGridMidiAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);
    title.setBounds(area.removeFromTop(36));
    area.removeFromTop(8);
    status.setBounds(area.removeFromTop(24));
    area.removeFromTop(12);
    loadButton.setBounds(area.removeFromTop(38).removeFromLeft(150));
    area.removeFromTop(16);
    variationLabel.setBounds(area.removeFromTop(22));
    variationSlider.setBounds(area.removeFromTop(36));
    area.removeFromTop(12);
    info.setBounds(area);
}

void GenreGridMidiAudioProcessorEditor::chooseMidi()
{
    chooser = std::make_unique<juce::FileChooser>("Select a MIDI file", juce::File{}, "*.mid;*.midi");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (file != juce::File{} && processor.loadMidiFile(file))
                status.setText(processor.getLoadedFileName() + "  •  " + juce::String(processor.getPhraseLengthBeats(), 1) + " beats", juce::dontSendNotification);
            else if (file != juce::File{})
                status.setText("Could not read MIDI file", juce::dontSendNotification);
        });
}
