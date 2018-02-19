#include "ESIoPortAudio.h"

#include "ESEngine.h"

using namespace ESSynth;

ESIoPortAudio::ESIoPortAudio(ESEngine* engine)
    : pa_stream_(nullptr),
      engine_(engine),
      value_left_(0.0f),
      value_right_(0.0f),
      sample_rate_(0),
      buffer_size_(0) {}

ESInt32Type ESIoPortAudio::Initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        return -1;
    }

    SetCurrentInterface(this);

    return 0;
}

ESInt32Type ESIoPortAudio::OpenOutputInterface(ESInt32Type /* interfaceId */) {
    // TODO load interface according to interfaceId
    PaError err =
        Pa_OpenDefaultStream(&pa_stream_, 0, 2, paFloat32, 44100, 256, PortAudioCallback, this);
    if (err != paNoError) {
        return -1;
    }

    sample_rate_ = 44100;
    buffer_size_ = 256;
    return 0;
}

ESInt32Type ESIoPortAudio::Start() {
    PaError err = Pa_StartStream(pa_stream_);
    if (err != paNoError) {
        return -1;
    }

    return 0;
}

ESInt32Type ESIoPortAudio::Stop() {
    PaError err = Pa_StopStream(pa_stream_);
    if (err != paNoError) {
        return -1;
    }
    return 0;
}

void ESIoPortAudio::WriteOutput(ESInt32Type outputId, ESFloatType value) {
    if (outputId == 0) {
        value_left_ = value;
    } else if (outputId == 1) {
        value_right_ = value;
    }
}

ESInt32Type ESIoPortAudio::GetBufferSize() { return buffer_size_; }

ESInt32Type ESIoPortAudio::GetSampleRate() { return sample_rate_; }

std::map<ESInt32Type, std::string> ESIoPortAudio::ListInputInterfaces() {
    std::map<ESInt32Type, std::string> result;
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->name && deviceInfo->maxInputChannels) {
            result[i] = deviceInfo->name;
        }
    }
    return result;
}

std::map<ESInt32Type, std::string> ESIoPortAudio::ListOutputInterfaces() {
    std::map<ESInt32Type, std::string> result;
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->name && deviceInfo->maxOutputChannels) {
            result[i] = deviceInfo->name;
        }
    }
    return result;
}

int ESIoPortAudio::PortAudioCallback(const void*, void* output, unsigned long frameCount,
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

