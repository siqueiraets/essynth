#ifndef ESMODULEMIDINOTE_H_
#define ESMODULEMIDINOTE_H_

#include "ESEngine.h"
#include "ESMidiInterface.h"

namespace ESSynth {

// TODO: Avoid using a global pointer
ESMidiInterface* gMidiInterface = nullptr;

enum class ESModuleMidiNoteInputs { MidiEvent };

enum class ESModuleMidiNoteOutputs { MidiNote, MidiVelocity, MidiGate };

struct ESModuleMidiNote
    : ESModule<ESModuleMidiNote, ESModuleMidiNoteInputs, ESModuleMidiNoteOutputs> {
    static constexpr ESInputList GetInputList() {
        return {MakeInput(ESDataType::Integer, "MidiEvent", TIn::MidiEvent)};
    }

    static constexpr ESOutputList GetOutputList() {
        return {MakeOutput(ESDataType::Integer, "MidiNote", TOut::MidiNote),
                MakeOutput(ESDataType::Integer, "MidiVelocity", TOut::MidiVelocity),
                MakeOutput(ESDataType::Integer, "MidiGate", TOut::MidiGate)};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

    static void Initialize(ESModuleRuntimeData*, ESData*, ESMidiInterface* midiInterface) {
        gMidiInterface = midiInterface;
    }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0 || !gMidiInterface) {
            return 0;
        }

        ESInt32Type note;
        ESInt32Type velocity;
        ESInt32Type gate;
        if (gMidiInterface->GetNoteEvent(note, velocity, gate)) {
            WriteOutput<TOut::MidiNote>(outputs, note);
            WriteOutput<TOut::MidiVelocity>(outputs, velocity);
            WriteOutput<TOut::MidiGate>(outputs, gate);
        }

        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEMIDINOTE_H_ */
