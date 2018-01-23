#ifndef ESMODULEMIDINOTE_H_
#define ESMODULEMIDINOTE_H_

#include "ESEngine.h"
#include "ESMidiInterface.h"

namespace ESSynth {

// TODO: Avoid using a global pointer
ESMidiInterface* gMidiInterface = nullptr;

struct ESModuleMidiNote {
    static constexpr ESInt32Type num_inputs = 1;
    static constexpr ESInt32Type num_outputs = 3;
    static constexpr ESInt32Type num_internals = 0;

    static void Initialize(ESModuleRuntimeData*, ESData*, ESMidiInterface* midiInterface) {
        gMidiInterface = midiInterface;
    }

    static ESInt32Type Process(const ESData*, ESOutput* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0 || !gMidiInterface) {
            return 0;
        }

        ESInt32Type note;
        ESInt32Type velocity;
        ESInt32Type gate;
        if(gMidiInterface->GetNoteEvent(note, velocity, gate)) {
            WriteOutput(0, outputs, note);
            WriteOutput(1, outputs, velocity);
            WriteOutput(2, outputs, gate);
        }

        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEMIDINOTE_H_ */
