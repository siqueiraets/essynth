#ifndef ESMIDIINTERFACE_H_
#define ESMIDIINTERFACE_H_

#include "ESDataTypes.h"

#include <map>
#include <string>

namespace ESSynth {

class ESMidiInterface {
   public:
    virtual ESInt32Type Initialize() = 0;
    virtual ESInt32Type OpenInputInterface(ESInt32Type interfaceId) = 0;
    virtual ESInt32Type Start() = 0;
    virtual ESInt32Type Stop() = 0;
    virtual ESInt32Type GetNoteEvent(ESInt32Type& note, ESInt32Type& velocity,
                                     ESInt32Type& gate) = 0;
    virtual ESInt32Type GetEventCount() = 0;
    virtual void RefreshEvents() = 0;
    virtual std::map<ESInt32Type, std::string> ListInputInterfaces() = 0;
    virtual std::map<ESInt32Type, std::string> ListOutputInterfaces() = 0;

    static ESMidiInterface* GetCurrentInterface();

   protected:
    void SetCurrentInterface(ESMidiInterface* interface);

   private:
    static ESMidiInterface* current_;
};

}  // namespace ESSynth

#endif /* ESMIDIINTERFACE_H_ */
