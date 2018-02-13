#ifndef ESIOPORTMIDI_H_
#define ESIOPORTMIDI_H_

#include <portmidi.h>

#include "ESMidiInterface.h"

namespace ESSynth {
const static ESInt32Type kPortMidiEventCount = 256;

class ESIoPortMidi : public ESMidiInterface {
   public:
    ESIoPortMidi();

    virtual ESInt32Type Initialize() override;
    virtual ESInt32Type OpenInputInterface(ESInt32Type interfaceId) override;
    virtual ESInt32Type GetNoteEvent(ESInt32Type& note, ESInt32Type& velocity,
                                     ESInt32Type& gate) override;
    virtual void RefreshEvents() override;
    virtual ESInt32Type GetEventCount() override;

   private:
    PmEvent events_[kPortMidiEventCount];
    ESInt32Type event_count_;
    ESInt32Type events_processed_;
    PortMidiStream* midi_stream_;
};

}  // namespace ESSynth

#endif /* ESIOPORTMIDI_H_ */
