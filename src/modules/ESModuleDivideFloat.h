#ifndef ESMODULEDIVIDEFLOAT_H_
#define ESMODULEDIVIDEFLOAT_H_

#include "ESEngine.h"

namespace ESSynth {

struct ESModuleDivideFloat {
    static constexpr ESInt32Type num_inputs = 2;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 0;

    static ESInt32Type Process(const ESData* inputs, ESOutput* outputs, ESData*,
                        const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        if (inputs[1].data_float > 0) {
            WriteOutput(0, outputs, inputs[0].data_float / inputs[1].data_float);
        }
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEDIVIDEFLOAT_H_ */
