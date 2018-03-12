#ifndef ESMODULEGATE_H_
#define ESMODULEGATE_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleGateInputs { Gate, EventIn };

enum class ESModuleGateOutputs { EventOut };

enum class ESModuleGateInternals { LastGate };

struct ESModuleGate
    : ESModule<ESModuleGate, ESModuleGateInputs, ESModuleGateOutputs, ESModuleGateInternals> {
    static std::string GetModuleName() { return "Gate"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "Gate", TIn::Gate),
                          MakeInput(ESDataType::Opaque, "EventIn", TIn::EventIn));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Opaque, "EventOut", TOut::EventOut));
    }

    static constexpr auto GetInternalList() {
        return MakeIoList(MakeInternal(ESDataType::Integer, "LastGate", TInt::LastGate));
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        Internal<TInt::LastGate>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        if (flags & InputFlag(TIn::Gate)) {
            Internal<TInt::LastGate>(internals) = Input<TIn::Gate>(inputs);
        }

        if (flags & InputFlag(TIn::EventIn)) {
            if (Internal<TInt::LastGate>(internals)) {
                WriteOutput<TOut::EventOut>(outputs, Input<TIn::EventIn>(inputs));
            }
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEGATE_H_ */
