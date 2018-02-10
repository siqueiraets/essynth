#ifndef ESMODULESUBTRACTINT_H_
#define ESMODULESUBTRACTINT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleSubtractIntInputs { In1, In2 };

enum class ESModuleSubtractIntOutputs { Out1 };

struct ESModuleSubtractInt
    : ESModule<ESModuleSubtractInt, ESModuleSubtractIntInputs, ESModuleSubtractIntOutputs> {
    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Integer, "In1", TIn::In1),
                MakeInput(ESDataType::Integer, "In2", TIn::In2)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Integer, "Out", TOut::Out1)}};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

    static void Initialize(ESModuleRuntimeData* data, ESData* internals) {}

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::Out1>(outputs, Input<TIn::In1>(inputs) - Input<TIn::In2>(inputs));
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULESUBTRACTINT_H_ */
