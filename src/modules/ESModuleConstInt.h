#ifndef ESMODULECONSTINT_H_
#define ESMODULECONSTINT_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleConstIntOutputs { ConstValue };

enum class ESModuleConstIntInternals { ConstValue };

struct ESModuleConstInt : ESModule<ESModuleConstInt, ESEmptyKeyType, ESModuleConstIntOutputs,
                                   ESModuleConstIntInternals> {
    static constexpr ESInputList GetInputList() { return {}; }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Integer, "ConstValue", TOut::ConstValue)}};
    }

    static constexpr ESOutputList GetInternalList() {
        return {{MakeInternal(ESDataType::Integer, "ConstValue", TInt::ConstValue)}};
    }

    static void Initialize(ESModuleRuntimeData* data, ESData* internals, ESInt32Type value) {
        Internal<TInt::ConstValue>(internals) = value;
        data->flags |= 1;
    }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput<TOut::ConstValue>(outputs, internals[0].data_int32);
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECONSTINT_H_ */
