#ifndef ESMODULEFLOATTOINT_H_
#define ESMODULEFLOATTOINT_H_

#include "ESEngine.h"

namespace ESSynth {

struct ESModuleFloatToInt {
    static constexpr ESInt32Type num_inputs = 1;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 0;

    static ESInt32Type Process(const ESData* inputs, ESOutput* outputs, ESData*,
                        const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        WriteOutput(0, outputs, (ESInt32Type)inputs[0].data_float);
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEFLOATTOINT_H_ */
