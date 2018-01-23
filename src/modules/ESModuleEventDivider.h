#ifndef ESMODULEEVENTDIVIDER_H_
#define ESMODULEEVENTDIVIDER_H_

#include "ESEngine.h"

namespace ESSynth {

struct ESModuleEventDivider {
    static constexpr ESInt32Type num_inputs = 1;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 2;

    static void Initialize(ESModuleRuntimeData*, ESData* internals, ESInt32Type count) {
        internals[0].data_int32 = count;
        internals[1].data_int32 = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutput* outputs, ESData* internals,
                        const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        if (++internals[1].data_int32 >= internals[0].data_int32) {
            WriteOutput(0, outputs, inputs[0]);
            internals[1].data_int32 = 0;
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEEVENTDIVIDER_H_ */
