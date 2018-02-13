#ifndef ESIOPORTAUDIO_H_
#define ESIOPORTAUDIO_H_

#include "ESAudioInterface.h"
#include "ESDataTypes.h"

#include <portaudio.h>

namespace ESSynth {

class ESEngine;
class ESIoPortAudio : public ESAudioInterface {
   public:
    ESIoPortAudio(ESEngine* engine);

    virtual ESInt32Type Initialize() override;
    virtual ESInt32Type OpenOutputInterface(ESInt32Type /* interfaceId */) override;
    virtual ESInt32Type Start() override;
    virtual void WriteOutput(ESInt32Type outputId, ESFloatType value) override;
    virtual ESInt32Type GetBufferSize() override;

    virtual ESInt32Type GetSampleRate() override;

   private:
    static int PortAudioCallback(const void*, void* output, unsigned long frameCount,
                                 const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
                                 void* userData);

    PaStream* pa_stream_;
    ESEngine* engine_;

    ESFloatType value_left_;
    ESFloatType value_right_;

    ESInt32Type sample_rate_;
    ESInt32Type buffer_size_;
};

}  // namespace ESSynth

#endif /* ESIOPORTAUDIO_H_ */
