#ifndef ESMIDIINTERFACE_H_
#define ESMIDIINTERFACE_H_

#include "ESDataTypes.h"

namespace ESSynth {

class ESMidiInterface {
   public:

    virtual ESInt32Type Initialize() = 0;
    virtual ESInt32Type OpenInputInterface(ESInt32Type interfaceId) = 0;
    virtual ESInt32Type GetNoteEvent(ESInt32Type& note, ESInt32Type& velocity, ESInt32Type& gate) = 0;
    virtual ESInt32Type GetEventCount() = 0;
    virtual void RefreshEvents() = 0;
};

}  // namespace ESSynth

#endif /* ESMIDIINTERFACE_H_ */
