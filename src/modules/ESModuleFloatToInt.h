#ifndef ESMODULEFLOATTOINT_H_
#define ESMODULEFLOATTOINT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleFloatToIntInputs { FloatValue };

enum class ESModuleFloatToIntOutputs { IntValue };

struct ESModuleFloatToInt
    : ESModule<ESModuleFloatToInt, ESModuleFloatToIntInputs, ESModuleFloatToIntOutputs> {
    static std::string GetModuleName() { return "Float to Integer"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Float, "FloatValue", TIn::FloatValue));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Integer, "IntValue", TOut::IntValue));
    }

    static constexpr auto GetInternalList() { return MakeIoList(); }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::IntValue>(outputs, (ESInt32Type)Input<TIn::FloatValue>(inputs));
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEFLOATTOINT_H_ */
