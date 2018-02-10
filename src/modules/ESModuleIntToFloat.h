#ifndef ESMODULEINTTOFLOAT_H_
#define ESMODULEINTTOFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleIntToFloatInputs { IntValue };

enum class ESModuleIntToFloatOutputs { FloatValue };

struct ESModuleIntToFloat
    : ESModule<ESModuleIntToFloat, ESModuleIntToFloatInputs, ESModuleIntToFloatOutputs> {
    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Integer, "IntValue", TIn::IntValue)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Float, "FloatValue", TOut::FloatValue)}};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::FloatValue>(outputs, (ESFloatType)Input<TIn::IntValue>(inputs));
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEINTTOFLOAT_H_ */
