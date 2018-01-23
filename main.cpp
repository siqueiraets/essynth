#include <QApplication>
#include "esmainwindow_ui.h"

#include "ESEngine.h"
#include "ESIoPortAudio.h"
#include "ESIoPortMidi.h"

#include <iostream>

using namespace ESSynth;
extern void test_modules(ESEngine&, ESIoPortAudio&, ESIoPortMidi&);

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    ESMainWindowUI w;

    ESEngine engine;
    ESIoPortAudio audioInterface(&engine);
    ESIoPortMidi midiInterface;

    if (audioInterface.Initialize() != 0) {
        std::cerr << "Failed to start audio interface" << std::endl;
        return 1;
    }

    if (midiInterface.Initialize() != 0) {
        std::cerr << "Failed to start midi interface" << std::endl;
        return 1;
    }

    // TODO read interface index from configuration
    if (audioInterface.OpenOutputInterface(0) != 0) {
        std::cerr << "Failed to open audio output interface" << std::endl;
        return 1;
    }

    // TODO read interface index from configuration
    if (midiInterface.OpenInputInterface(3) != 0) {
        std::cerr << "Failed to start midi input interface" << std::endl;
        return 1;
    }

    test_modules(engine, audioInterface, midiInterface);

    engine.Prepare();
    audioInterface.Start();

    w.show();
    return a.exec();
}
