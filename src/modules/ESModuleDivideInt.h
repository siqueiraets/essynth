#ifndef ESMODULEDIVIDEINT_H_
#define ESMODULEDIVIDEINT_H_

#include "ESEngine.h"

namespace ESSynth {

struct ESModuleDivideInt {
    static constexpr ESInt32Type num_inputs = 2;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 0;

    static void Initialize(ESModuleRuntimeData* data, ESData* internals) {}

    static ESInt32Type Process(const ESData* inputs, ESOutput* outputs, ESData*,
                        const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        if (inputs[1].data_int32 > 0) {
            WriteOutput(0, outputs, inputs[0].data_int32 / inputs[1].data_int32);
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEDIVIDEINT_H_ */
