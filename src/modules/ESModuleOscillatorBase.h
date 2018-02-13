#ifndef ESMODULEOSCILLATORBASE_H_
#define ESMODULEOSCILLATORBASE_H_

#include "ESEngine.h"

#define ES_OSC_RESOLUTION 1024

// TODO sample rate needs to be read at runtime
#define SAMPLE_RATE 44100

namespace ESSynth {

enum class ESModuleOscillatorBaseInputs { Frequency, Clock };

enum class ESModuleOscillatorBaseOutputs { Amplitude };

enum class ESModuleOscillatorBaseInternals { Phase };

template <typename Type>
struct ESModuleOscillatorBase
    : ESModule<ESModuleOscillatorBase<Type>, ESModuleOscillatorBaseInputs,
               ESModuleOscillatorBaseOutputs, ESModuleOscillatorBaseInternals> {
    using BaseType = ESModule<ESModuleOscillatorBase<Type>, ESModuleOscillatorBaseInputs,
                              ESModuleOscillatorBaseOutputs, ESModuleOscillatorBaseInternals>;

    static ESFloatType osc_table[ES_OSC_RESOLUTION];
    static constexpr auto GetInputList() {
        return BaseType::MakeIoList(
            BaseType::MakeInput(ESDataType::Float, "Frequency", BaseType::TIn::Frequency),
            BaseType::MakeInput(ESDataType::Integer, "Clock", BaseType::TIn::Clock));
    }

    static constexpr auto GetOutputList() {
        return BaseType::MakeIoList(
            BaseType::MakeOutput(ESDataType::Float, "Amplitude", BaseType::TOut::Amplitude));
    }

    static constexpr auto GetInternalList() {
        return BaseType::MakeIoList(
            BaseType::MakeInternal(ESDataType::Float, "Phase", BaseType::TInt::Phase));
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (!(flags & BaseType::InputFlag(BaseType::TIn::Clock))) {
            return 0;
        }

        ESFloatType frequency = BaseType::template Input<BaseType::TIn::Frequency>(inputs);
        ESFloatType increment = (ES_OSC_RESOLUTION * frequency) / (ESFloatType)SAMPLE_RATE;
        ESFloatType phase = BaseType::template Internal<BaseType::TInt::Phase>(internals);
        phase += increment;
        if (phase >= (ESFloatType)ES_OSC_RESOLUTION) {
            phase -= (ESFloatType)ES_OSC_RESOLUTION;
        }

        ESFloatType value = osc_table[(ESInt32Type)phase];
        BaseType::template Internal<BaseType::TInt::Phase>(internals) = phase;

        BaseType::template WriteOutput<BaseType::TOut::Amplitude>(outputs, value);

        return 0;
    }
};

template <typename Type>
ESFloatType ESModuleOscillatorBase<Type>::osc_table[ES_OSC_RESOLUTION];

}  // namespace ESSynth

#endif /* ESMODULEOSCILLATORBASE_H_ */
