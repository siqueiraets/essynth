#ifndef ESMODULETRIANGLEOSC_H_
#define ESMODULETRIANGLEOSC_H_

#include "ESEngine.h"
#include "ESModuleOscillatorBase.h"

namespace ESSynth {

struct ESModuleTriangleOsc : public ESModuleOscillatorBase<ESModuleTriangleOsc> {
    static std::string GetModuleName() { return "Triangle OSC"; }
    using BaseType = ESModuleOscillatorBase<ESModuleTriangleOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < (ES_OSC_RESOLUTION / 2); ++i) {
            BaseType::osc_table[i] = (2.0f / (ES_OSC_RESOLUTION / 2) * i) - 1.0f;
        }

        for (ESInt32Type i = (ES_OSC_RESOLUTION / 2); i < ES_OSC_RESOLUTION; ++i) {
            BaseType::osc_table[i] = BaseType::osc_table[ES_OSC_RESOLUTION - i - 1];
        }
    }
};

}  // namespace ESSynth

#endif /* ESMODULETRIANGLEOSC_H_ */
