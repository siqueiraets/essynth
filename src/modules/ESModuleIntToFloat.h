#ifndef ESMODULEINTTOFLOAT_H_
#define ESMODULEINTTOFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleIntToFloatInputs { IntValue };

enum class ESModuleIntToFloatOutputs { FloatValue };

struct ESModuleIntToFloat
    : ESModule<ESModuleIntToFloat, ESModuleIntToFloatInputs, ESModuleIntToFloatOutputs> {
    static std::string GetModuleName() { return "Integer to Float"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "IntValue", TIn::IntValue));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Float, "FloatValue", TOut::FloatValue));
    }

    static constexpr auto GetInternalList() { return MakeIoList(); }

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
