#ifndef ESMODULESAWOSC_H_
#define ESMODULESAWOSC_H_

#include "ESEngine.h"
#include "ESModuleOscillatorBase.h"

namespace ESSynth {

struct ESModuleSawOsc : public ESModuleOscillatorBase<ESModuleSawOsc> {
    static std::string GetModuleName() { return "Saw OSC"; }
    using BaseType = ESModuleOscillatorBase<ESModuleSawOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < ES_OSC_RESOLUTION; ++i) {
            BaseType::osc_table[i] = (2.0f / ES_OSC_RESOLUTION) * i - 1.0f;
        }
    }
};

}  // namespace ESSynth

#endif /* ESMODULESAWOSC_H_ */
