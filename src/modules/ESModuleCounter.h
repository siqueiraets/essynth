#ifndef ESMODULECOUNTER_H_
#define ESMODULECOUNTER_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleCounterOutputs { Value };

enum class ESModuleCounterInternals { InitialValue, CurrentValue, FinalValue };

struct ESModuleCounter
    : ESModule<ESModuleCounter, ESEmptyKeyType, ESModuleCounterOutputs, ESModuleCounterInternals> {
    static constexpr ESInputList GetInputList() { return {}; }

    static constexpr ESOutputList GetOutputList() {
        return {MakeOutput(ESDataType::Integer, "Value", TOut::Value)};
    }

    static constexpr ESOutputList GetInternalList() {
        return {MakeInternal(ESDataType::Integer, "InitialValue", TInt::InitialValue),
                MakeInternal(ESDataType::Integer, "CurrentValue", TInt::CurrentValue),
                MakeInternal(ESDataType::Integer, "FinalValue", TInt::FinalValue)};
    }

    static void Initialize(ESModuleRuntimeData* runtimeData, ESData* internals,
                           ESInt32Type initialValue, ESInt32Type finalValue) {
        Internal<TInt::InitialValue>(internals) = initialValue;
        Internal<TInt::CurrentValue>(internals) = initialValue;
        Internal<TInt::FinalValue>(internals) = finalValue;
        runtimeData->flags |= 1;
    }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::Value>(outputs, Internal<TInt::CurrentValue>(internals));
        Internal<TInt::CurrentValue>(internals)++;
        if (Internal<TInt::CurrentValue>(internals) > Internal<TInt::FinalValue>(internals)) {
            Internal<TInt::CurrentValue>(internals) = Internal<TInt::InitialValue>(internals);
        }
        return 1;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECOUNTER_H_ */
