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

    ESInt32Type Initialize() override;
    ESInt32Type OpenOutputInterface(ESInt32Type /* interfaceId */) override;
    ESInt32Type Start() override;
    ESInt32Type Stop() override;
    void WriteOutput(ESInt32Type outputId, ESFloatType value) override;
    ESInt32Type GetBufferSize() override;
    ESInt32Type GetSampleRate() override;

    std::map<ESInt32Type, std::string> ListInputInterfaces() override;
    std::map<ESInt32Type, std::string> ListOutputInterfaces() override;

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
