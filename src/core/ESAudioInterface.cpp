#include "ESAudioInterface.h"

using namespace ESSynth;

ESAudioInterface* ESAudioInterface::current_ = nullptr;

ESAudioInterface* ESAudioInterface::GetCurrentInterface() { return current_; }

void ESAudioInterface::SetCurrentInterface(ESAudioInterface* interface) { current_ = interface; }
