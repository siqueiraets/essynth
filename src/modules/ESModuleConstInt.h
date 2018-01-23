#ifndef ESMODULECONSTINT_H_
#define ESMODULECONSTINT_H_

#include "ESEngine.h"

namespace ESSynth {

struct ESModuleConstInt {
    static constexpr ESInt32Type num_inputs = 0;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 1;

    static void Initialize(ESModuleRuntimeData* data, ESData* internals, ESInt32Type value) {
        internals[0].data_int32 = value;
        data->flags |= 1;
    }

    static ESInt32Type Process(const ESData*, ESOutput* outputs, ESData* internals,
                        const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput(0, outputs, internals[0].data_int32);
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECONSTINT_H_ */
