#ifndef ESMODULEMIDIEVENT_H_
#define ESMODULEMIDIEVENT_H_

#include "ESEngine.h"
#include "ESMidiInterface.h"

namespace ESSynth {

enum class ESModuleMidiEventInputs { ClockEvent };

enum class ESModuleMidiEventOutputs { MidiEvent };

struct ESModuleMidiEvent
    : ESModule<ESModuleMidiEvent, ESModuleMidiEventInputs, ESModuleMidiEventOutputs> {
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "ClockEvent", TIn::ClockEvent));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Integer, "MidiEvent", TOut::MidiEvent));
    }

    static constexpr auto GetInternalList() { return MakeIoList(); }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        auto midiInterface = ESMidiInterface::GetCurrentInterface();
        if (!midiInterface) {
            return 0;
        }

        midiInterface->RefreshEvents();
        ESInt32Type eventCount = midiInterface->GetEventCount();
        if (eventCount > 0) {
            WriteOutput<TOut::MidiEvent>(outputs, eventCount);
        }

        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEMIDIEVENT_H_ */
