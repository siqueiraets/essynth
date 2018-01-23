#ifndef ESIOPORTAUDIO_H_
#define ESIOPORTAUDIO_H_

#include <portaudio.h>
#include "ESAudioInterface.h"
#include "ESEngine.h"

namespace ESSynth {
class ESIoPortAudio : public ESAudioInterface {
   public:
    ESIoPortAudio(ESEngine* engine)
        : pa_stream_(nullptr),
          engine_(engine),
          value_left_(0.0f),
          value_right_(0.0f),
          sample_rate_(0),
          buffer_size_(0) {}

    virtual ESInt32Type Initialize() override {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            return -1;
        }

        return 0;
    }

    virtual ESInt32Type OpenOutputInterface(ESInt32Type /* interfaceId */ ) override {
        PaError err =
            Pa_OpenDefaultStream(&pa_stream_, 0,    /* no input channels */
                                 2,                 /* stereo output */
                                 paFloat32,         /* 32 bit floating point output */
                                 44100, 256,        /* frames per buffer, i.e. the number
                                                                 of sample frames that PortAudio will
                                                                 request from the callback. Many apps
                                                                 may want to use
                                                                 paFramesPerBufferUnspecified, which
                                                                 tells PortAudio to pick the best,
                                                                 possibly changing, buffer size.*/
                                 PortAudioCallback, /* this is your callback function */
                                 this);             /*This is a pointer that will be passed to
                                                               your callback*/

        if (err != paNoError) {
            return -1;
        }

        sample_rate_ = 44100;
        buffer_size_ = 256;
        return 0;
    }

    virtual ESInt32Type Start() override {
        PaError err = Pa_StartStream(pa_stream_);
        if (err != paNoError) {
            return -1;
        }

        return 0;
    }

    virtual void WriteOutput(ESInt32Type outputId, ESFloatType value) override {
        if (outputId == 0) {
            value_left_ = value;
        } else if (outputId == 1) {
            value_right_ = value;
        }
    }

    virtual ESInt32Type GetBufferSize() override { return buffer_size_; }

    virtual ESInt32Type GetSampleRate() override { return sample_rate_; }

   private:
    static int PortAudioCallback(const void*, void* output, unsigned long frameCount,
                                 const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags,
                                 void* userData) {
        ESIoPortAudio* ptrThis = static_cast<ESIoPortAudio*>(userData);
        float* out = (float*)output;

        for (unsigned long i = 0; i < frameCount; i++) {
            ptrThis->engine_->Process();
            *out++ = ptrThis->value_left_;
            *out++ = ptrThis->value_right_;
        }

        return paContinue;
    }

    PaStream* pa_stream_;
    ESEngine* engine_;

    ESFloatType value_left_;
    ESFloatType value_right_;

    ESInt32Type sample_rate_;
    ESInt32Type buffer_size_;
};

}  // namespace ESSynth

#endif /* ESIOPORTAUDIO_H_ */
