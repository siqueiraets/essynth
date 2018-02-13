#ifndef ESMODULESUBTRACTFLOAT_H_
#define ESMODULESUBTRACTFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleSubtractFloatInputs { In1, In2 };

enum class ESModuleSubtractFloatOutputs { Out1 };

struct ESModuleSubtractFloat
    : ESModule<ESModuleSubtractFloat, ESModuleSubtractFloatInputs, ESModuleSubtractFloatOutputs> {
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Float, "In1", TIn::In1),
                          MakeInput(ESDataType::Float, "In2", TIn::In2));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Float, "Out", TOut::Out1));
    }

    static constexpr auto GetInternalList() { return MakeIoList(); }

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

#endif /* ESMODULESUBTRACTFLOAT_H_ */
