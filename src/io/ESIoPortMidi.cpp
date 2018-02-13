#include "ESIoPortMidi.h"

using namespace ESSynth;

ESIoPortMidi::ESIoPortMidi() : event_count_(0), events_processed_(0) {}

ESInt32Type ESIoPortMidi::Initialize() {
    PmError err = Pm_Initialize();
    if (err != pmNoError) {
        return -1;
    }

    SetCurrentInterface(this);

    return 0;
}

ESInt32Type ESIoPortMidi::OpenInputInterface(ESInt32Type interfaceId) {
    PmError err = Pm_OpenInput(&midi_stream_, interfaceId, nullptr, 256, nullptr, nullptr);
    if (err != pmNoError) {
        return -1;
    }
    return 0;
}

ESInt32Type ESIoPortMidi::GetNoteEvent(ESInt32Type& note, ESInt32Type& velocity,
                                       ESInt32Type& gate) {
    if (events_processed_ >= event_count_) {
        return 0;
    }

    if ((Pm_MessageStatus(events_[events_processed_].message) & 0xF0) == 0x80) {
        note = Pm_MessageData1(events_[events_processed_].message);
        velocity = Pm_MessageData2(events_[events_processed_].message);
        gate = 0;
        return 1;
    } else if ((Pm_MessageStatus(events_[events_processed_].message) & 0xF0) == 0x90) {
        note = Pm_MessageData1(events_[events_processed_].message);
        velocity = Pm_MessageData2(events_[events_processed_].message);
        gate = 1;
        return 1;
    }

    return 0;
}

void ESIoPortMidi::RefreshEvents() {
    if (events_processed_ >= event_count_) {
        events_processed_ = 0;
        event_count_ = 0;
        auto ret = Pm_Read(midi_stream_, events_, kPortMidiEventCount);
        if (ret > 0) {
            event_count_ = ret;
        }
    } else {
        events_processed_++;
    }
}

ESInt32Type ESIoPortMidi::GetEventCount() { return event_count_ - events_processed_; }

