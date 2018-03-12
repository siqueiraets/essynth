#ifndef ESMODULEEVENTDIVIDER_H_
#define ESMODULEEVENTDIVIDER_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleEventDividerInputs { Divisor, InEvent };

enum class ESModuleEventDividerOutputs { OutEvent };

enum class ESModuleEventDividerInternals { CurrentValue };

struct ESModuleEventDivider : ESModule<ESModuleEventDivider, ESModuleEventDividerInputs,
                                       ESModuleEventDividerOutputs, ESModuleEventDividerInternals> {
    static std::string GetModuleName() { return "Event Divider"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "Divisor", TIn::Divisor),
                          MakeInput(ESDataType::Opaque, "InEvent", TIn::InEvent));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Opaque, "OutEvent", TOut::OutEvent));
    }

    static constexpr auto GetInternalList() {
        return MakeIoList(MakeInternal(ESDataType::Integer, "CurrentValue", TInt::CurrentValue));
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        Internal<TInt::CurrentValue>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        if (++Internal<TInt::CurrentValue>(internals) >= Input<TIn::Divisor>(inputs)) {
            WriteOutput<TOut::OutEvent>(outputs, Input<TIn::InEvent>(inputs));
            Internal<TInt::CurrentValue>(internals) = 0;
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEEVENTDIVIDER_H_ */
