#ifndef ESMODULEADDINT_H_
#define ESMODULEADDINT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleAddIntInputs { In1, In2 };

enum class ESModuleAddIntOutputs { Out1 };

struct ESModuleAddInt : ESModule<ESModuleAddInt, ESModuleAddIntInputs, ESModuleAddIntOutputs> {
    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Integer, "In1", TIn::In1),
                MakeInput(ESDataType::Integer, "In2", TIn::In2)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Integer, "Out", TOut::Out1)}};
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

#endif /* ESMODULEADDINT_H_ */
