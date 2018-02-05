#ifndef ESMODULEEVENTDIVIDER_H_
#define ESMODULEEVENTDIVIDER_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleEventDividerInputs { InEvent };

enum class ESModuleEventDividerOutputs { OutEvent };

enum class ESModuleEventDividerInternals { MaxCount, CurrentValue };

struct ESModuleEventDivider : ESModule<ESModuleEventDivider, ESModuleEventDividerInputs,
                                       ESModuleEventDividerOutputs, ESModuleEventDividerInternals> {
    static constexpr ESInputList GetInputList() {
        return {MakeInput(ESDataType::Opaque, "InEvent", TIn::InEvent)};
    }

    static constexpr ESOutputList GetOutputList() {
        return {MakeOutput(ESDataType::Opaque, "OutEvent", TOut::OutEvent)};
    }

    static constexpr ESOutputList GetInternalList() {
        return {MakeInternal(ESDataType::Integer, "MaxCount", TInt::MaxCount),
                MakeInternal(ESDataType::Integer, "CurrentValue", TInt::CurrentValue)};
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals, ESInt32Type count) {
        Internal<TInt::MaxCount>(internals) = count;
        Internal<TInt::CurrentValue>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        if (++Internal<TInt::CurrentValue>(internals) >= Internal<TInt::MaxCount>(internals)) {
            WriteOutput<TOut::OutEvent>(outputs, Input<TIn::InEvent>(inputs));
            Internal<TInt::CurrentValue>(internals) = 0;
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEEVENTDIVIDER_H_ */
