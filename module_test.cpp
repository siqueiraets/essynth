#include "ESEngine.h"
#include "ESModuleAddFloat.h"
#include "ESModuleAddInt.h"
#include "ESModuleAudioOut.h"
#include "ESModuleCounter.h"
#include "ESModuleDivideFloat.h"
#include "ESModuleDivideInt.h"
#include "ESModuleEventDivider.h"
#include "ESModuleFloatToInt.h"
#include "ESModuleIntToFloat.h"
#include "ESModuleMidiEvent.h"
#include "ESModuleMidiNote.h"
#include "ESModuleMultiplyFloat.h"
#include "ESModuleMultiplyInt.h"
#include "ESModuleOscillatorBase.h"
#include "ESModuleSawOsc.h"
#include "ESModuleSineOsc.h"
#include "ESModuleSquareOsc.h"
#include "ESModuleSubtractFloat.h"
#include "ESModuleSubtractInt.h"
#include "ESModuleTriangleOsc.h"
#include "ESModuleGate.h"
#include "ESModuleNoteToFreq.h"

#include "ESIoPortAudio.h"
#include "ESIoPortMidi.h"

#include <math.h>

using namespace ESSynth;

enum class ESModuleInputSelectInputs { In1, In2, In3, Switch };

enum class ESModuleInputSelectOutputs { Out };

enum class ESModuleInputSelectInternals { CurSelection };

struct ESModuleInputSelect : ESModule<ESModuleInputSelect, ESModuleInputSelectInputs,
                                      ESModuleInputSelectOutputs, ESModuleInputSelectInternals> {
    static constexpr auto GetInputList() {
        return MakeIoList(MakeInput(ESDataType::Opaque, "In1", TIn::In1),
                          MakeInput(ESDataType::Opaque, "In2", TIn::In2),
                          MakeInput(ESDataType::Opaque, "In3", TIn::In3),
                          MakeInput(ESDataType::Opaque, "Switch", TIn::Switch));
    }

    static constexpr auto GetOutputList() {
        return MakeIoList(MakeOutput(ESDataType::Opaque, "Out", TOut::Out));
    }

    static constexpr auto GetInternalList() {
        return MakeIoList(MakeInternal(ESDataType::Integer, "CurSelection", TInt::CurSelection));
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        Internal<TInt::CurSelection>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        if (flags & InputFlag(TIn::Switch)) {
            Internal<TInt::CurSelection>(internals)++;
            if (Internal<TInt::CurSelection>(internals) >= 3) {
                Internal<TInt::CurSelection>(internals) = 0;
            }
        }

        switch (Internal<TInt::CurSelection>(internals)) {
            case 0:

                if (flags & InputFlag(TIn::In1)) {
                    WriteOutput<TOut::Out>(outputs, Input<TIn::In1>(inputs));
                }
                break;
            case 1:
                if (flags & InputFlag(TIn::In2)) {
                    WriteOutput<TOut::Out>(outputs, Input<TIn::In2>(inputs));
                }
                break;
            case 2:
            default:
                if (flags & InputFlag(TIn::In3)) {
                    WriteOutput<TOut::Out>(outputs, Input<TIn::In3>(inputs));
                }
                break;
        }

        return 0;
    }
};


void test_modules(ESEngine& engine, ESIoPortAudio& audioInterface) {
    ESInt32Type srcounter = engine.CreateModule<ESModuleCounter>();
    ESInt32Type oscilator = engine.CreateModule<ESModuleSawOsc>();
    ESInt32Type oscilator2 = engine.CreateModule<ESModuleSineOsc>();
    ESInt32Type oscilator3 = engine.CreateModule<ESModuleSquareOsc>();
    ESInt32Type oscilator4 = engine.CreateModule<ESModuleTriangleOsc>();
    ESInt32Type divider = engine.CreateModule<ESModuleEventDivider>();
    ESInt32Type changer = engine.CreateModule<ESModuleEventDivider>();
    ESInt32Type audioLeft = engine.CreateModule<ESModuleAudioOut>();
    ESInt32Type audioRight = engine.CreateModule<ESModuleAudioOut>();

    ESInt32Type midiEvent = engine.CreateModule<ESModuleMidiEvent>();
    ESInt32Type midiNote = engine.CreateModule<ESModuleMidiNote>();

    ESInt32Type noteOnFilter = engine.CreateModule<ESModuleGate>();

    ESInt32Type noteToFreq = engine.CreateModule<ESModuleNoteToFreq>();

    ESInt32Type scaler = engine.CreateModule<ESModuleMultiplyFloat>();
    ESInt32Type inputSelect = engine.CreateModule<ESModuleInputSelect>();

    engine.SetConstData(srcounter, 0, ESData{.data_int32 = 0});
    engine.SetConstData(srcounter, 1, ESData{.data_int32 = audioInterface.GetSampleRate() - 1});

    engine.SetConstData(divider, 0, ESData{.data_int32 = audioInterface.GetSampleRate() / 100});
    engine.SetConstData(changer, 0, ESData{.data_int32 = audioInterface.GetSampleRate() * 5});

    engine.SetConstData(audioLeft, 0, ESData{.data_int32 = 0});
    engine.SetConstData(audioRight, 0, ESData{.data_int32 = 1});

    engine.Connect(srcounter, 0, oscilator, 1);
    engine.Connect(srcounter, 0, oscilator2, 1);
    engine.Connect(srcounter, 0, oscilator3, 1);
    engine.Connect(srcounter, 0, oscilator4, 1);
    engine.Connect(srcounter, 0, divider, 1);
    engine.Connect(srcounter, 0, changer, 1);

    engine.Connect(divider, 0, midiEvent, 0);
    engine.Connect(midiEvent, 0, midiNote, 0);
    engine.Connect(midiNote, 2, noteOnFilter, 0);
    engine.Connect(midiNote, 0, noteOnFilter, 1);
    engine.Connect(noteOnFilter, 0, noteToFreq, 0);

    engine.Connect(noteToFreq, 0, oscilator, 0);
    engine.Connect(noteToFreq, 0, oscilator2, 0);
    engine.Connect(noteToFreq, 0, oscilator3, 0);
    engine.Connect(noteToFreq, 0, oscilator4, 0);

    engine.Connect(oscilator, 0, inputSelect, 0);
    engine.Connect(oscilator2, 0, inputSelect, 1);
    engine.Connect(oscilator4, 0, inputSelect, 2);
    engine.Connect(changer, 0, inputSelect, 3);

    engine.Connect(inputSelect, 0, scaler, 0);

    engine.SetConstData(scaler, 1, ESData{.data_float = 0.60f});
    engine.Connect(scaler, 0, audioLeft, 1);
    engine.Connect(scaler, 0, audioRight, 1);
}
