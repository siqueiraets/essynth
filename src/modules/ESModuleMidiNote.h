#ifndef ESMODULEMIDINOTE_H_
#define ESMODULEMIDINOTE_H_

#include "ESEngine.h"
#include "ESMidiInterface.h"

namespace ESSynth {

enum class ESModuleMidiNoteInputs { MidiEvent };

enum class ESModuleMidiNoteOutputs { MidiNote, MidiVelocity, MidiGate };

struct ESModuleMidiNote
    : ESModule<ESModuleMidiNote, ESModuleMidiNoteInputs, ESModuleMidiNoteOutputs> {
    static std::string GetModuleName() { return "Midi Note"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "MidiEvent", TIn::MidiEvent));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Integer, "MidiNote", TOut::MidiNote),
                          MakeOutput(ESDataType::Integer, "MidiVelocity", TOut::MidiVelocity),
                          MakeOutput(ESDataType::Integer, "MidiGate", TOut::MidiGate));
    }

    static constexpr auto GetInternalList() { return MakeIoList(); }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        ESInt32Type note;
        ESInt32Type velocity;
        ESInt32Type gate;

        auto midiInterface = ESMidiInterface::GetCurrentInterface();
        if (!midiInterface) {
            return 0;
        }

        if (midiInterface->GetNoteEvent(note, velocity, gate)) {
            WriteOutput<TOut::MidiNote>(outputs, note);
            WriteOutput<TOut::MidiVelocity>(outputs, velocity);
            WriteOutput<TOut::MidiGate>(outputs, gate);
        }

        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEMIDINOTE_H_ */
