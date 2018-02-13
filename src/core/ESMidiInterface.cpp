#include "ESMidiInterface.h"

using namespace ESSynth;

ESMidiInterface* ESMidiInterface::current_ = nullptr;

ESMidiInterface* ESMidiInterface::GetCurrentInterface() { return current_; }

void ESMidiInterface::SetCurrentInterface(ESMidiInterface* interface) { current_ = interface; }
