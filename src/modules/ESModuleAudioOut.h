#ifndef ESMODULEAUDIOOUT_H_
#define ESMODULEAUDIOOUT_H_

#include "ESAudioInterface.h"
#include "ESEngine.h"

namespace ESSynth {

// TODO avoid using global variable
ESAudioInterface* gAudioInterface = nullptr;

struct ESModuleAudioOut {
    static constexpr ESInt32Type num_inputs = 1;
    static constexpr ESInt32Type num_outputs = 0;
    static constexpr ESInt32Type num_internals = 1;

    static void Initialize(ESModuleRuntimeData*, ESData* internals, ESInt32Type outputId,
                           ESAudioInterface* audioInterface) {
        gAudioInterface = audioInterface;
        internals[0].data_int32 = outputId;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutput*, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        gAudioInterface->WriteOutput(internals[0].data_int32, inputs[0].data_float);
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEAUDIOOUT_H_ */
