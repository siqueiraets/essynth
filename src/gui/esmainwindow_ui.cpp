#include "esmainwindow_ui.h"
#include <QApplication>
#include <QWheelEvent>
#include "esmodule_ui.h"
#include "ui_esmainwindow.h"

#include "ESEngine.h"
#include "ESModuleAddFloat.h"
#include "ESModuleAudioOut.h"
#include "ESModuleCounter.h"
#include "ESModuleEventDivider.h"
#include "ESModuleIntToFloat.h"
#include "ESModuleMidiEvent.h"
#include "ESModuleMidiNote.h"
#include "ESModuleMultiplyFloat.h"
#include "ESModuleSubtractFloat.h"

#include "ESIoPortAudio.h"
#include "ESIoPortMidi.h"

#include <iostream>

using namespace ESSynth;
ESEngine engine;
ESIoPortAudio audioInterface(&engine);
ESIoPortMidi midiInterface;

class QAction;
class QMenu;

QMenu* menu_view_;
QMenu* menu_module_;

QAction* add_module_2;
QAction* add_module_4;
QAction* del_module;

template <typename Type>
static void SetupActionModuleType(QMenu* menu, ESDesignerViewUI* view) {
    QAction* new_action;
    QString module_name = typeid(Type).name();
    new_action = new QAction(QString("Add module ") + module_name, nullptr);
    menu->addAction(new_action);

    auto connectFn = [module_name, new_action, view]() {
        auto point = view->GetLastPoint();
        auto moduleId = engine.CreateModule<Type>();
        view->AddModule(point.x(), point.y(), Type::GetNumInputs(), Type::GetNumOutputs(),
                        module_name, moduleId);
    };

    QObject::connect(new_action, &QAction::triggered, new_action, connectFn);
};

template <typename... Types>
struct SetupActionModules {
    void operator()(QMenu* menu, ESDesignerViewUI* view) {
        int unused[]{(SetupActionModuleType<Types>(menu, view), 0)...};
        (void)unused;
    }
};

ESMainWindowUI::ESMainWindowUI(QWidget* parent) : QMainWindow(parent), ui_(new Ui::ESMainWindowUI) {
    ui_->setupUi(this);
    designer_view_ = new ESDesignerViewUI(ui_->frameDesigner);

    menu_view_ = new QMenu(designer_view_);

    menu_module_ = new QMenu(designer_view_);
    del_module = new QAction("Del module", designer_view_);
    menu_module_->addAction(del_module);

    if (audioInterface.Initialize() != 0) {
        std::cerr << "Failed to start audio interface" << std::endl;
        exit(1);
    }

    if (midiInterface.Initialize() != 0) {
        std::cerr << "Failed to start midi interface" << std::endl;
        exit(1);
    }

    auto midiInterfaces = midiInterface.ListInputInterfaces();
    auto audioInterfaces = audioInterface.ListOutputInterfaces();
    for (auto& midiIf : midiInterfaces) {
        ui_->comboMidiInterfaces->addItem(midiIf.second.c_str(), midiIf.first);
    }

    for (auto& audioIf : audioInterfaces) {
        ui_->comboAudioInterfaces->addItem(audioIf.second.c_str(), audioIf.first);
    }

    SetupActions();

    connect(designer_view_, &ESDesignerViewUI::MenuRequested, this, &ESMainWindowUI::MenuRequested);
    connect(designer_view_, SIGNAL(ModuleMenuRequested(int, int, ESModuleUI*)), this,
            SLOT(ModuleMenuRequested(int, int, ESModuleUI*)));
    connect(ui_->btnStart, SIGNAL(clicked()), this, SLOT(StartAudio()));
    connect(ui_->btnStop, SIGNAL(clicked()), this, SLOT(StopAudio()));
}

ESMainWindowUI::~ESMainWindowUI() {
    delete ui_;
    delete menu_view_;
    delete menu_module_;
}

void ESMainWindowUI::MenuRequested(int x, int y) {
    auto action = menu_view_->exec(designer_view_->mapToGlobal(QPoint(x, y)));
    if (action == add_module_2) {
        designer_view_->AddModule(x, y, 2, 2, "Module 2x2");
    } else if (action == add_module_4) {
        designer_view_->AddModule(x, y, 4, 4, "Module 4x4");
    }
}

void ESMainWindowUI::ModuleMenuRequested(int x, int y, ESModuleUI* module) {
    auto action = menu_module_->exec(designer_view_->mapToGlobal(QPoint(x, y)));
    if (action == del_module) {
        designer_view_->RemoveModule(module);
    }
}

void ESMainWindowUI::SetupActions() {
    // Setup menu options to create modules
    SetupActionModules<ESModuleAddFloat, ESModuleAudioOut, ESModuleCounter, ESModuleEventDivider,
                       ESModuleIntToFloat, ESModuleMidiEvent, ESModuleMidiNote,
                       ESModuleMultiplyFloat, ESModuleSubtractFloat>()(menu_view_, designer_view_);
}

void ESMainWindowUI::StartAudio() {
    engine.Prepare();

    if (audioInterface.OpenOutputInterface(ui_->comboAudioInterfaces->currentData().toInt()) != 0) {
        std::cerr << "Failed to open audio output interface" << std::endl;
        exit(1);
    }

    if (midiInterface.OpenInputInterface(ui_->comboMidiInterfaces->currentData().toInt()) != 0) {
        std::cerr << "Failed to start midi input interface" << std::endl;
        exit(1);
    }

    if (audioInterface.Start() != 0) {
        std::cerr << "Failed to start audio" << std::endl;
        exit(1);
    }

    if (midiInterface.Start() != 0) {
        std::cerr << "Failed to start midi" << std::endl;
        exit(1);
    }
}

void ESMainWindowUI::StopAudio() {
    audioInterface.Stop();
    midiInterface.Stop();
}
