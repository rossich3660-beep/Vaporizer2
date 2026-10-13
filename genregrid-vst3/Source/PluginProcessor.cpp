#include "PluginProcessor.h"
#include "PluginEditor.h"

GenreGridMidiAudioProcessor::GenreGridMidiAudioProcessor()
    : AudioProcessor(BusesProperties())
{
}

void GenreGridMidiAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sampleRateHz = sampleRate > 0.0 ? sampleRate : 44100.0;
    fallbackPpq = 0.0;
}

bool GenreGridMidiAudioProcessor::loadMidiFile(const juce::File& file)
{
    if (!file.existsAsFile() || !file.hasFileExtension("mid;midi"))
        return false;
    auto input = file.createInputStream();
    if (input == nullptr)
        return false;

    juce::MidiFile midi;
    if (!midi.readFrom(*input))
        return false;

    const int ticksPerQuarter = midi.getTimeFormat();
    if (ticksPerQuarter <= 0)
        return false;

    std::vector<MidiEvent> parsed;
    double lastBeat = 0.0;
    for (int track = 0; track < midi.getNumTracks(); ++track)
    {
        auto* sequence = midi.getTrack(track);
        if (sequence == nullptr) continue;
        for (int i = 0; i < sequence->getNumEvents(); ++i)
        {
            const auto* event = sequence->getEventPointer(i);
            if (event == nullptr) continue;
            const auto& message = event->message;
            if (!message.isNoteOn() && !message.isNoteOff())
                continue;
            const double beat = juce::jmax(0.0, message.getTimeStamp() / (double) ticksPerQuarter);
            parsed.push_back({ beat, message });
            lastBeat = juce::jmax(lastBeat, beat);
        }
    }

    if (parsed.empty()) return false;
    std::stable_sort(parsed.begin(), parsed.end(), [](const MidiEvent& a, const MidiEvent& b)
    {
        if (a.beat != b.beat) return a.beat < b.beat;
        return a.message.isNoteOff() && b.message.isNoteOn();
    });

    const double length = juce::jmax(1.0, std::ceil((lastBeat + 0.001) * 4.0) / 4.0);
    const juce::ScopedLock lock(midiLock);
    events = std::move(parsed);
    phraseLengthBeats = length;
    loadedFileName = file.getFileName();
    fallbackPpq = 0.0;
    return true;
}

juce::String GenreGridMidiAudioProcessor::getLoadedFileName() const
{
    const juce::ScopedLock lock(midiLock);
    return loadedFileName;
}

void GenreGridMidiAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    midi.clear();

    const juce::ScopedLock lock(midiLock);
    if (events.empty() || sampleRateHz <= 0.0)
        return;

    double bpm = 120.0;
    double blockStartPpq = fallbackPpq;
    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            if (auto hostBpm = position->getBpm(); hostBpm.hasValue() && *hostBpm > 0.0)
                bpm = *hostBpm;
            if (auto ppq = position->getPpqPosition(); ppq.hasValue())
                blockStartPpq = *ppq;
        }
    }

    const double beatsPerSample = bpm / (60.0 * sampleRateHz);
    const double blockEndPpq = blockStartPpq + buffer.getNumSamples() * beatsPerSample;
    const double length = juce::jmax(1.0, phraseLengthBeats);
    const float amount = variation.load();
    const int blockSamples = buffer.getNumSamples();

    for (const auto& event : events)
    {
        const double baseBeat = event.beat;
        const long long firstLoop = (long long) std::floor((blockStartPpq - baseBeat) / length);
        for (long long loop = juce::jmax(0LL, firstLoop); loop <= firstLoop + 2; ++loop)
        {
            double targetBeat = baseBeat + (double) loop * length;
            if (targetBeat < blockStartPpq - 1.0e-7 || targetBeat >= blockEndPpq)
                continue;

            auto message = event.message;
            if (message.isNoteOn())
            {
                const auto seed = (uint32_t) (loop * 7919 + (long long)(baseBeat * 997.0) + message.getNoteNumber() * 31);
                juce::Random rng(seed);
                const int velocity = message.getVelocity();
                const int variedVelocity = juce::jlimit(1, 127, velocity + (int) std::round((rng.nextFloat() * 2.0f - 1.0f) * 22.0f * amount));
                message.setVelocity(variedVelocity);
                if (amount > 0.01f && rng.nextFloat() < amount * 0.25f)
                    targetBeat += (rng.nextFloat() - 0.5) * 0.08 * amount;
            }

            const int sample = juce::jlimit(0, juce::jmax(0, blockSamples - 1),
                (int) std::round((targetBeat - blockStartPpq) / beatsPerSample));
            midi.addEvent(message, sample);
        }
    }

    fallbackPpq = blockEndPpq;
}

juce::AudioProcessorEditor* GenreGridMidiAudioProcessor::createEditor()
{
    return new GenreGridMidiAudioProcessorEditor(*this);
}

void GenreGridMidiAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
    stream.writeString(getLoadedFileName());
    stream.writeFloat(getVariation());
}

void GenreGridMidiAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, (size_t) sizeInBytes, false);
    loadedFileName = stream.readString();
    variation.store(juce::jlimit(0.0f, 1.0f, stream.readFloat()));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GenreGridMidiAudioProcessor();
}
