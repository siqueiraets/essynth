#include "ESEngine.h"
#include "ESModuleAddFloat.h"
#include "ESModuleAudioOut.h"
#include "ESModuleConstFloat.h"
#include "ESModuleConstInt.h"
#include "ESModuleCounter.h"
#include "ESModuleEventDivider.h"
#include "ESModuleIntToFloat.h"
#include "ESModuleMidiEvent.h"
#include "ESModuleMidiNote.h"
#include "ESModuleMultiplyFloat.h"
#include "ESModuleOscillatorBase.h"
#include "ESModuleSubtractFloat.h"

#include "ESIoPortAudio.h"
#include "ESIoPortMidi.h"

#include <math.h>

using namespace ESSynth;

struct ESModuleSineOsc : public ESModuleOscillatorBase<ESModuleSineOsc> {
    using BaseType = ESModuleOscillatorBase<ESModuleSineOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < ES_OSC_RESOLUTION; ++i) {
            BaseType::osc_table[i] = (ESFloatType)sin(2.0f * M_PI * (i + 1) / ES_OSC_RESOLUTION);
        }
    }
};

struct ESModuleSquareOsc : public ESModuleOscillatorBase<ESModuleSquareOsc> {
    using BaseType = ESModuleOscillatorBase<ESModuleSquareOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < ES_OSC_RESOLUTION; ++i) {
            if (i <= (ES_OSC_RESOLUTION / 2)) {
                BaseType::osc_table[i] = -1.0f;
            } else {
                BaseType::osc_table[i] = 1.0f;
            }
        }
    }
};

struct ESModuleSawOsc : public ESModuleOscillatorBase<ESModuleSawOsc> {
    using BaseType = ESModuleOscillatorBase<ESModuleSawOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < ES_OSC_RESOLUTION; ++i) {
            BaseType::osc_table[i] = (2.0f / ES_OSC_RESOLUTION) * i - 1.0f;
        }
    }
};

struct ESModuleTriangleOsc : public ESModuleOscillatorBase<ESModuleTriangleOsc> {
    using BaseType = ESModuleOscillatorBase<ESModuleTriangleOsc>;
    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        internals[0].data_float = 0.0f;

        for (ESInt32Type i = 0; i < (ES_OSC_RESOLUTION / 2); ++i) {
            BaseType::osc_table[i] = (2.0f / (ES_OSC_RESOLUTION / 2) * i) - 1.0f;
        }

        for (ESInt32Type i = (ES_OSC_RESOLUTION / 2); i < ES_OSC_RESOLUTION; ++i) {
            BaseType::osc_table[i] = BaseType::osc_table[ES_OSC_RESOLUTION - i - 1];
        }
    }
};

enum class ESModuleInputSelectInputs { In1, In2, In3, In4 };

enum class ESModuleInputSelectOutputs { Out };

enum class ESModuleInputSelectInternals { CurSelection };

struct ESModuleInputSelect : ESModule<ESModuleInputSelect, ESModuleInputSelectInputs,
                                      ESModuleInputSelectOutputs, ESModuleInputSelectInternals> {
    static constexpr ESInt32Type num_inputs = 4;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 1;

    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Opaque, "In1", TIn::In1),
                MakeInput(ESDataType::Opaque, "In2", TIn::In2),
                MakeInput(ESDataType::Opaque, "In3", TIn::In3),
                MakeInput(ESDataType::Opaque, "In4", TIn::In4)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Opaque, "Out", TOut::Out)}};
    }

    static constexpr ESInternalList GetInternalList() {
        return {{MakeInternal(ESDataType::Integer, "CurSelection", TInt::CurSelection)}};
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        Internal<TInt::CurSelection>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }

        if (flags & InputFlag(3)) {
            Internal<TInt::CurSelection>(internals)++;
            if (Internal<TInt::CurSelection>(internals) >= 3) {
                Internal<TInt::CurSelection>(internals) = 0;
            }
            return 0;
        }

        switch (Internal<TInt::CurSelection>(internals)) {
            case 0:
                WriteOutput<TOut::Out>(outputs, Input<TIn::In1>(inputs));
                break;
            case 1:
                WriteOutput<TOut::Out>(outputs, Input<TIn::In2>(inputs));
                break;
            case 2:
                WriteOutput<TOut::Out>(outputs, Input<TIn::In3>(inputs));
                break;
            case 3:
            default:
                WriteOutput<TOut::Out>(outputs, Input<TIn::In4>(inputs));
                break;
        }

        return 0;
    }
};

enum class ESModuleNoteToFreqInputs { MidiNote };

enum class ESModuleNoteToFreqOutputs { Frequency };

struct ESModuleNoteToFreq
    : ESModule<ESModuleNoteToFreq, ESModuleNoteToFreqInputs, ESModuleNoteToFreqOutputs> {
    static constexpr ESInt32Type num_inputs = 1;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 0;

    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Integer, "MidiNote", TIn::MidiNote)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Float, "Frequency", TOut::Frequency)}};
    }

    static constexpr ESOutputList GetInternalList() { return {}; }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData*,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        ESFloatType notes[] = {
            0.0f,     0.0f,     0.0f,     0.0f,     0.0f,     0.0f,     0.0f,     0.0f,
            0.0f,     0.0f,     0.0f,     0.0f,     16.35f,   17.32f,   18.35f,   19.45f,
            20.6f,    21.83f,   23.12f,   24.5f,    25.96f,   27.5f,    29.14f,   30.87f,
            32.7f,    34.65f,   36.71f,   38.89f,   41.2f,    43.65f,   46.25f,   49.0f,
            51.91f,   55.0f,    58.27f,   61.74f,   65.41f,   69.3f,    73.42f,   77.78f,
            82.41f,   87.31f,   92.5f,    98.0f,    103.83f,  110.0f,   116.54f,  123.47f,
            130.81f,  138.59f,  146.83f,  155.56f,  164.81f,  174.61f,  185.0f,   196.0f,
            207.65f,  220.0f,   233.08f,  246.94f,  261.63f,  277.18f,  293.66f,  311.13f,
            329.63f,  349.23f,  369.99f,  392.0f,   415.3f,   440.0f,   466.16f,  493.88f,
            523.25f,  554.37f,  587.33f,  622.25f,  659.25f,  698.46f,  739.99f,  783.99f,
            830.61f,  880.0f,   932.33f,  987.77f,  1046.5f,  1108.73f, 1174.66f, 1244.51f,
            1318.51f, 1396.91f, 1479.98f, 1567.98f, 1661.22f, 1760.0f,  1864.66f, 1975.53f,
            2093.0f,  2217.46f, 2349.32f, 2489.02f, 2637.02f, 2793.83f, 2959.96f, 3135.96f,
            3322.44f, 3520.0f,  3729.31f, 3951.07f, 4186.01f, 4434.92f, 4698.63f, 4978.03f,
            5274.04f, 5587.65f, 5919.91f, 6271.93f, 6644.88f, 7040.0f,  7458.62f, 7902.13f};
        WriteOutput<TOut::Frequency>(outputs, (ESFloatType)notes[Input<TIn::MidiNote>(inputs)]);
        return 0;
    }
};

enum class ESModuleGateInputs { Gate, EventIn };

enum class ESModuleGateOutputs { EventOut };

enum class ESModuleGateInternals { LastGate };

struct ESModuleGate
    : ESModule<ESModuleGate, ESModuleGateInputs, ESModuleGateOutputs, ESModuleGateInternals> {
    static constexpr ESInt32Type num_inputs = 2;
    static constexpr ESInt32Type num_outputs = 1;
    static constexpr ESInt32Type num_internals = 1;

    static constexpr ESInputList GetInputList() {
        return {{MakeInput(ESDataType::Integer, "Gate", TIn::Gate),
                MakeInput(ESDataType::Opaque, "EventIn", TIn::EventIn)}};
    }

    static constexpr ESOutputList GetOutputList() {
        return {{MakeOutput(ESDataType::Opaque, "EventOut", TOut::EventOut)}};
    }

    static constexpr ESOutputList GetInternalList() {
        return {{MakeInternal(ESDataType::Integer, "LastGate", TInt::LastGate)}};
    }

    static void Initialize(ESModuleRuntimeData*, ESData* internals) {
        Internal<TInt::LastGate>(internals) = 0;
    }

    static ESInt32Type Process(const ESData* inputs, ESOutputRuntime* outputs, ESData* internals,
                               const ESInt32Type& flags) {
        if (flags == 0) {
            return 0;
        }
        if (flags & InputFlag(0)) {
            Internal<TInt::LastGate>(internals) = Input<TIn::Gate>(inputs);
        }

        if (flags & InputFlag(1)) {
            if (Internal<TInt::LastGate>(internals)) {
                WriteOutput<TOut::EventOut>(outputs, Input<TIn::EventIn>(inputs));
            }
        }
        return 0;
    }
};

void test_modules(ESEngine& engine, ESIoPortAudio& audioInterface, ESIoPortMidi& midiInterface) {
    ESInt32Type srcounter =
        engine.CreateModule<ESModuleCounter>(0, audioInterface.GetSampleRate() - 1);
    ESInt32Type oscilator = engine.CreateModule<ESModuleSawOsc>();
    ESInt32Type oscilator2 = engine.CreateModule<ESModuleSineOsc>();
    ESInt32Type oscilator3 = engine.CreateModule<ESModuleSquareOsc>();
    ESInt32Type oscilator4 = engine.CreateModule<ESModuleTriangleOsc>();
    ESInt32Type divider =
        engine.CreateModule<ESModuleEventDivider>(audioInterface.GetSampleRate() / 100);
    ESInt32Type changer =
        engine.CreateModule<ESModuleEventDivider>(audioInterface.GetSampleRate() * 5);
    ESInt32Type audioLeft = engine.CreateModule<ESModuleAudioOut>(0, &audioInterface);
    ESInt32Type audioRight = engine.CreateModule<ESModuleAudioOut>(1, &audioInterface);

    ESInt32Type midiEvent = engine.CreateModule<ESModuleMidiEvent>(&midiInterface);
    ESInt32Type midiNote = engine.CreateModule<ESModuleMidiNote>(&midiInterface);

    ESInt32Type noteOnFilter = engine.CreateModule<ESModuleGate>();

    ESInt32Type noteToFreq = engine.CreateModule<ESModuleNoteToFreq>();

    ESInt32Type scaler = engine.CreateModule<ESModuleMultiplyFloat>();
    ESInt32Type scaleConst = engine.CreateModule<ESModuleConstFloat>(0.60f);
    ESInt32Type inputSelect = engine.CreateModule<ESModuleInputSelect>();

    engine.Connect(srcounter, 0, oscilator, 1);
    engine.Connect(srcounter, 0, oscilator2, 1);
    engine.Connect(srcounter, 0, oscilator3, 1);
    engine.Connect(srcounter, 0, oscilator4, 1);
    engine.Connect(srcounter, 0, divider, 0);
    engine.Connect(srcounter, 0, changer, 0);

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
    engine.Connect(scaleConst, 0, scaler, 1);
    engine.Connect(scaler, 0, audioLeft, 0);
    engine.Connect(scaler, 0, audioRight, 0);
}
