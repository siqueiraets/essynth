#ifndef ESIOPORTMIDI_H_
#define ESIOPORTMIDI_H_

#include <portmidi.h>
#include "ESMidiInterface.h"

namespace ESSynth {
const static ESInt32Type kPortMidiEventCount = 256;

class ESIoPortMidi : public ESMidiInterface {
   public:
    ESIoPortMidi() : event_count_(0), events_processed_(0) {}

    virtual ESInt32Type Initialize() {
        PmError err = Pm_Initialize();
        if (err != pmNoError) {
            return -1;
        }

        return 0;
    }

    virtual ESInt32Type OpenInputInterface(ESInt32Type interfaceId) {
        PmError err = Pm_OpenInput(&midi_stream_, interfaceId, nullptr, 256, nullptr, nullptr);
        if (err != pmNoError) {
            return -1;
        }
        return 0;
    }

    virtual ESInt32Type GetNoteEvent(ESInt32Type& note, ESInt32Type& velocity, ESInt32Type& gate) {
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

    virtual void RefreshEvents() {
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

    virtual ESInt32Type GetEventCount() { return event_count_ - events_processed_; }

   private:
    PmEvent events_[kPortMidiEventCount];
    ESInt32Type event_count_;
    ESInt32Type events_processed_;
    PortMidiStream* midi_stream_;
};

}  // namespace ESSynth

#endif /* ESIOPORTMIDI_H_ */
