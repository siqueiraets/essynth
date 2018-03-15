#ifndef ESMODULECLOCK_H_
#define ESMODULECLOCK_H_

#include "ESEngine.h"

namespace ESSynth {

enum class ESModuleClockOutputs { Clock };

struct ESModuleClock : ESModule<ESModuleClock, ESEmptyKeyType, ESModuleClockOutputs> {
    static std::string GetModuleName() { return "Clock"; }
    static constexpr auto GetInputList() { return MakeIoList(); }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Integer, "Clock", TOut::Clock));
    }

    static constexpr auto GetInternalList() { return MakeIoList(); }

    static ESInt32Type Process(const ESData*, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type&) {
        WriteOutput<TOut::Clock>(outputs, 1);
        return 0;
    }
};

}  // namespace ESSynth

#endif /* ESMODULECLOCK_H_ */
