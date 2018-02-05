#ifndef ESMODULEAUDIOOUT_H_
#define ESMODULEAUDIOOUT_H_

#include "ESAudioInterface.h"
#include "ESEngine.h"

namespace ESSynth {

// TODO avoid using global variable
ESAudioInterface* gAudioInterface = nullptr;

enum class ESModuleAudioOutInputs { Amplitude };

enum class ESModuleAudioOutInternals { OutputId };

struct ESModuleAudioOut : ESModule<ESModuleAudioOut, ESModuleAudioOutInputs, ESEmptyKeyType,
                                   ESModuleAudioOutInternals> {
    static constexpr ESInputList GetInputList() {
        return {MakeInput(ESDataType::Float, "Amplitude", TIn::Amplitude)};
    }

    static constexpr ESOutputList GetOutputList() { return {}; }

    static constexpr ESOutputList GetInternalList() {
        return {MakeInternal(ESDataType::Integer, "OutputId", TInt::OutputId)};
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals, ESInt32Type outputId,
                           ESAudioInterface* audioInterface) {
        gAudioInterface = audioInterface;
        Internal<TInt::OutputId>(internals) = outputId;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime*, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        gAudioInterface->WriteOutput(Internal<TInt::OutputId>(internals),
                                     Input<TIn::Amplitude>(inputs));
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEAUDIOOUT_H_ */
