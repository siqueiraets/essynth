#ifndef ESMODULEDIVIDEFLOAT_H_
#define ESMODULEDIVIDEFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleDivideFloatInputs { In1, In2 };

enum class ESModuleDivideFloatOutputs { Out1 };

struct ESModuleDivideFloat
    : ESModule<ESModuleDivideFloat, ESModuleDivideFloatInputs, ESModuleDivideFloatOutputs> {
    static std::string GetModuleName() { return "Divide Float"; }
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
        if (Input<TIn::In2>(inputs) > 0) {
            WriteOutput<TOut::Out1>(outputs, Input<TIn::In1>(inputs) / Input<TIn::In2>(inputs));
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEDIVIDEFLOAT_H_ */
