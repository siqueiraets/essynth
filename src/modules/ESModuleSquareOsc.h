#ifndef ESMODULESQUAREOSC_H_
#define ESMODULESQUAREOSC_H_

#include "ESEngine.h"
#include "ESModuleOscillatorBase.h"

namespace ESSynth {

struct ESModuleSquareOsc : public ESModuleOscillatorBase<ESModuleSquareOsc> {
    static std::string GetModuleName() { return "Square OSC"; }
    using BaseType = ESModuleOscillatorBase<ESModuleSquareOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < ES_OSC_RESOLUTION; ++i) {
            if (i <= (ES_OSC_RESOLUTION / 2)) {
                BaseType::osc_table[i] = -1.0f;
            } else {
                BaseType::osc_table[i] = 1.0f;
            }
        }
    }
};

}  // namespace ESSynth

#endif /* ESMODULESQUAREOSC_H_ */
