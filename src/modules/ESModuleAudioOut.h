#ifndef ESMODULEAUDIOOUT_H_
#define ESMODULEAUDIOOUT_H_

#include "ESAudioInterface.h"
#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleAudioOutInputs { OutputId, Amplitude };

struct ESModuleAudioOut : ESModule<ESModuleAudioOut, ESModuleAudioOutInputs> {
    static std::string GetModuleName() { return "Audio Output"; }
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Integer, "OutputId", TIn::OutputId),
                          MakeInput(ESDataType::Float, "Amplitude", TIn::Amplitude));
    }

    static constexpr auto GetOutputList() { return MakeIoList(); }

    static constexpr auto GetInternalList() { return MakeIoList(); }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime*, ESData*,
                               const ESInt32Type& flags) {
        if ((flags & InputFlag(TIn::Amplitude)) == 0) {
            return 0;
        }

        auto audioInterface = ESAudioInterface::GetCurrentInterface();
        audioInterface->WriteOutput(Input<TIn::OutputId>(inputs), Input<TIn::Amplitude>(inputs));
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULEAUDIOOUT_H_ */
