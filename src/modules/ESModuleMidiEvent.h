#ifndef ESMODULEMIDIEVENT_H_
#define ESMODULEMIDIEVENT_H_

#include "ESEngine.h"
#include "ESMidiInterface.h"

namespace ESSynth {

// TODO: Avoid using a global pointer
ESMidiInterface* gEventMidiInterface = nullptr;

enum class ESModuleMidiEventInputs { ClockEvent };

enum class ESModuleMidiEventOutputs { MidiEvent };

struct ESModuleMidiEvent
    : ESModule<ESModuleMidiEvent, ESModuleMidiEventInputs, ESModuleMidiEventOutputs> {
    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Integer, "ClockEvent", TIn::ClockEvent)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Integer, "MidiEvent", TOut::MidiEvent)}};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

    static void Initialize(ESModuleRuntimeData*, ESData*, ESMidiInterface* midiInterface) {
        gEventMidiInterface = midiInterface;
    }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0 || !gEventMidiInterface) {
            return 0;
        }

        gEventMidiInterface->RefreshEvents();
        ESInt32Type eventCount = gEventMidiInterface->GetEventCount();
        if (eventCount > 0) {
            WriteOutput<TOut::MidiEvent>(outputs, eventCount);
        }

        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEMIDIEVENT_H_ */
