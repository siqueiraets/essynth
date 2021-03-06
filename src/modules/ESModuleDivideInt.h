#ifndef ESMODULEDIVIDEINT_H_
#define ESMODULEDIVIDEINT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleDivideIntInputs { In1, In2 };

enum class ESModuleDivideIntOutputs { Out1 };

struct ESModuleDivideInt
    : ESModule<ESModuleDivideInt, ESModuleDivideIntInputs, ESModuleDivideIntOutputs> {
    static std::string GetModuleName() { return "Divide Integer"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "In1", TIn::In1),
                          MakeInput(ESDataType::Integer, "In2", TIn::In2));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Integer, "Out", TOut::Out1));
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

#endif /* ESMODULEDIVIDEINT_H_ */
