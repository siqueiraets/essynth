#ifndef ESMODULEOSCILLATORBASE_H_
#define ESMODULEOSCILLATORBASE_H_

#include "ESEngine.h"

#define ES_OSC_RESOLUTION 1024

// TODO sample rate needs to be read at runtime
#define SAMPLE_RATE 44100

namespace ESSynth {

template <typename Type>
struct ESModuleOscillatorBase {
    static ESFloatType osc_table[ES_OSC_RESOLUTION];
    static constexpr ESInt32Type num_inputs = 2;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 1;

    static ESInt32Type Process(const ESData* inputs, ESOutput* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        ESFloatType increment =
            (ES_OSC_RESOLUTION * inputs[0].data_float) / (ESFloatType)SAMPLE_RATE;
        ESFloatType phase = internals[0].data_float;
        phase += increment;
        if (phase >= (ESFloatType)ES_OSC_RESOLUTION) {
            phase -= (ESFloatType)ES_OSC_RESOLUTION;
        }

        ESFloatType value = osc_table[(ESInt32Type)phase];
        internals[0].data_float = phase;

        WriteOutput(0, outputs, value);

        return 0;
    }
};

template <typename Type>
ESFloatType ESModuleOscillatorBase<Type>::osc_table[ES_OSC_RESOLUTION];

}  // namespace ESSynth

#endif /* ESMODULEOSCILLATORBASE_H_ */
