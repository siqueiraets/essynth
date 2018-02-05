#ifndef ESMODULEFLOATTOINT_H_
#define ESMODULEFLOATTOINT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleFloatToIntInputs { FloatValue };

enum class ESModuleFloatToIntOutputs { IntValue };

struct ESModuleFloatToInt
    : ESModule<ESModuleFloatToInt, ESModuleFloatToIntInputs, ESModuleFloatToIntOutputs> {
    static constexpr ESInputList GetInputList() {
        return {MakeInput(ESDataType::Float, "FloatValue", TIn::FloatValue)};
    }

    static constexpr ESOutputList GetOutputList() {
        return {MakeOutput(ESDataType::Integer, "IntValue", TOut::IntValue)};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

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
