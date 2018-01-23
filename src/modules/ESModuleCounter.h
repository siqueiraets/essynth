#ifndef ESMODULECOUNTER_H_
#define ESMODULECOUNTER_H_

#include "ESEngine.h"

namespace ESSynth {

struct ESModuleCounter {
    static constexpr ESInt32Type num_inputs = 0;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 3;

    static void Initialize(ESModuleRuntimeData* runtimeData, ESData* internals,
                           ESInt32Type initialValue, ESInt32Type finalValue) {
        internals[0].data_int32 = initialValue;
        internals[1].data_int32 = initialValue;
        internals[2].data_int32 = finalValue;
        runtimeData->flags |= 1;
    }

    static ESInt32Type Process(const ESData*, ESOutput* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput(0, outputs, internals[0].data_int32);
        internals[0].data_int32++;
        if (internals[0].data_int32 > internals[2].data_int32) {
            internals[0].data_int32 = internals[1].data_int32;
        }
        return 1;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECOUNTER_H_ */
