#ifndef ESMODULECONSTFLOAT_H_
#define ESMODULECONSTFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleConstFloatOutputs { ConstValue };

enum class ESModuleConstFloatInternals { ConstValue };

struct ESModuleConstFloat : ESModule<ESModuleConstFloat, ESEmptyKeyType, ESModuleConstFloatOutputs,
                                     ESModuleConstFloatInternals> {
    static constexpr ESInputList GetInputList() { return {}; }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Float, "ConstValue", TOut::ConstValue)}};
    }

    static constexpr ESOutputList GetInternalList() {
        return {{MakeInternal(ESDataType::Float, "ConstValue", TInt::ConstValue)}};
    }

    static void Initialize(ESModuleRuntimeData* data, ESData* internals, ESFloatType value) {
        Internal<TInt::ConstValue>(internals) = value;
        data->flags |= 1;
    }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::ConstValue>(outputs, internals[0].data_float);
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECONSTFLOAT_H_ */
