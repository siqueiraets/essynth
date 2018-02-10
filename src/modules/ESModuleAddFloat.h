#ifndef ESMODULEADDFLOAT_H_
#define ESMODULEADDFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleAddFloatInputs { In1, In2 };

enum class ESModuleAddFloatOutputs { Out1 };

struct ESModuleAddFloat
    : ESModule<ESModuleAddFloat, ESModuleAddFloatInputs, ESModuleAddFloatOutputs> {
    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Float, "In1", TIn::In1),
                MakeInput(ESDataType::Float, "In2", TIn::In2)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Float, "Out", TOut::Out1)}};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::Out1>(outputs, Input<TIn::In1>(inputs) + Input<TIn::In2>(inputs));
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEADDFLOAT_H_ */
