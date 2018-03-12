#ifndef ESMODULESINEOSC_H_
#define ESMODULESINEOSC_H_

#include "ESEngine.h"
#include "ESModuleOscillatorBase.h"

#include <math.h>

namespace ESSynth {

struct ESModuleSineOsc : public ESModuleOscillatorBase<ESModuleSineOsc> {
    static std::string GetModuleName() { return "Sine OSC"; }
    using BaseType = ESModuleOscillatorBase<ESModuleSineOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < ES_OSC_RESOLUTION; ++i) {
            BaseType::osc_table[i] = (ESFloatType)sin(2.0f * M_PI * (i + 1) / ES_OSC_RESOLUTION);
        }
    }
};

}  // namespace ESSynth

#endif /* ESMODULESINEOSC_H_ */
