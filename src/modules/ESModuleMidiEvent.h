#ifndef ESMODULEMIDIEVENT_H_
#define ESMODULEMIDIEVENT_H_

#include "ESEngine.h"
#include "ESMidiInterface.h"

namespace ESSynth {

// TODO: Avoid using a global pointer
ESMidiInterface* gEventMidiInterface = nullptr;

struct ESModuleMidiEvent {
    static constexpr ESInt32Type num_inputs = 1;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 0;

    static void Initialize(ESModuleRuntimeData*, ESData*, ESMidiInterface* midiInterface) {
        gEventMidiInterface = midiInterface;
    }

    static ESInt32Type Process(const ESData*, ESOutput* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0 || !gEventMidiInterface) {
            return 0;
        }

        gEventMidiInterface->RefreshEvents();
        ESInt32Type eventCount = gEventMidiInterface->GetEventCount();
        if (eventCount > 0) {
            WriteOutput(0, outputs, eventCount);
        }

        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEMIDIEVENT_H_ */
