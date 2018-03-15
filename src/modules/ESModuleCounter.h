#ifndef ESMODULECOUNTER_H_
#define ESMODULECOUNTER_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleCounterInputs { InitialValue, FinalValue, Clock, Reset };

enum class ESModuleCounterOutputs { Value };

enum class ESModuleCounterInternals { CurrentValue };

struct ESModuleCounter : ESModule<ESModuleCounter, ESModuleCounterInputs, ESModuleCounterOutputs,
                                  ESModuleCounterInternals> {
    static std::string GetModuleName() { return "Counter"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "InitialValue", TIn::InitialValue),
                          MakeInput(ESDataType::Integer, "FinalValue", TIn::FinalValue),
                          MakeInput(ESDataType::Integer, "Clock", TIn::Clock),
                          MakeInput(ESDataType::Integer, "Reset", TIn::Reset));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Integer, "Value", TOut::Value));
    }

    static constexpr auto GetInternalList() {
        return MakeIoList(MakeInternal(ESDataType::Integer, "CurrentValue", TInt::CurrentValue));
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        Internal<TInt::CurrentValue>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags & InputFlag(TIn::InitialValue)) {
            Internal<TInt::CurrentValue>(internals) = Input<TIn::InitialValue>(inputs);
        }

        if (flags & InputFlag(TIn::Reset)) {
            Internal<TInt::CurrentValue>(internals) = Input<TIn::InitialValue>(inputs);
        }

        if (!(flags & InputFlag(TIn::Clock))) {
            return 0;
        }

        WriteOutput<TOut::Value>(outputs, Internal<TInt::CurrentValue>(internals));
        Internal<TInt::CurrentValue>(internals)++;
        if (Internal<TInt::CurrentValue>(internals) > Input<TIn::FinalValue>(inputs)) {
            Internal<TInt::CurrentValue>(internals) = Input<TIn::InitialValue>(inputs);
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECOUNTER_H_ */
