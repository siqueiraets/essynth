#ifndef ESAUDIOINTERFACE_H_
#define ESAUDIOINTERFACE_H_

#include "ESDataTypes.h"

namespace ESSynth {

class ESAudioInterface {
   public:
    virtual ESInt32Type Initialize() = 0;
    virtual ESInt32Type OpenOutputInterface(ESInt32Type interfaceId) = 0;
    virtual ESInt32Type Start() = 0;
    virtual void WriteOutput(ESInt32Type outputId, ESFloatType value) = 0;
    virtual ESInt32Type GetBufferSize() = 0;
    virtual ESInt32Type GetSampleRate() = 0;
};

}  // namespace ESSynth

#endif /* ESAUDIOINTERFACE_H_ */
