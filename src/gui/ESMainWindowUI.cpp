#include "ESMainWindowUI.h"
#include "ui_ESMainWindowUI.h"

#include <QApplication>
#include <QGraphicsView>
#include <QWheelEvent>

#include "ESDesignerScene.h"
#include "ESModuleUI.h"

#include "ESEngine.h"
#include "ESIoPortAudio.h"
#include "ESIoPortMidi.h"
#include "ESModuleAddFloat.h"
#include "ESModuleAudioOut.h"
#include "ESModuleCounter.h"
#include "ESModuleEventDivider.h"
#include "ESModuleIntToFloat.h"
#include "ESModuleMidiEvent.h"
#include "ESModuleMidiNote.h"
#include "ESModuleMultiplyFloat.h"
#include "ESModuleSubtractFloat.h"

#include <iostream>

using namespace ESSynth;
ESEngine engine;
ESIoPortAudio audioInterface(&engine);
ESIoPortMidi midiInterface;

class QAction;
class QMenu;

QMenu* menu_view_;
QMenu* menu_module_;

QAction* del_module;
ESDesignerScene* designer_scene;

template <typename Type>
static void SetupActionModuleType(QMenu* menu, QGraphicsView* view) {
    QAction* new_action;
    QString module_name = typeid(Type).name();
    new_action = new QAction(QString("Add module ") + module_name, nullptr);
    menu->addAction(new_action);

    auto connectFn = [menu, module_name, view]() {
        auto moduleId = engine.CreateModule<Type>();
        auto scenePoint = view->mapToScene(view->mapFromGlobal(menu->pos()));
        ESModuleInfoUI moduleInfo{moduleId, Type::GetNumInputs(), Type::GetNumOutputs(),
                                  module_name};
        designer_scene->AddModule(scenePoint.x(), scenePoint.y(), moduleInfo);
    };

    QObject::connect(new_action, &QAction::triggered, new_action, connectFn);
};

template <typename... Types>
struct SetupActionModules {
    void operator()(QMenu* menu, QGraphicsView* view) {
        int unused[]{(SetupActionModuleType<Types>(menu, view), 0)...};
        (void)unused;
    }
};

ESMainWindowUI::ESMainWindowUI(QWidget* parent) : QMainWindow(parent), ui_(new Ui::ESMainWindowUI) {
    ui_->setupUi(this);
    menu_view_ = new QMenu(this);
    menu_module_ = new QMenu(this);

    designer_view_ = new QGraphicsView(this);
    designer_scene = new ESDesignerScene(menu_view_, menu_module_, this);
    del_module = new QAction("Del module", designer_view_);

    designer_view_->setScene(designer_scene);
    menu_module_->addAction(del_module);

    ui_->frameDesigner->layout()->addWidget(designer_view_);

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

    connect(designer_scene, SIGNAL(ModuleConnected(int, int, int, int)), this,
            SLOT(ModuleConnected(int, int, int, int)));
    connect(ui_->btnStart, SIGNAL(clicked()), this, SLOT(StartAudio()));
    connect(ui_->btnStop, SIGNAL(clicked()), this, SLOT(StopAudio()));
    connect(del_module, SIGNAL(triggered()), this, SLOT(HandleRemoveModule()));
}

ESMainWindowUI::~ESMainWindowUI() {
    delete ui_;
    delete menu_view_;
    delete menu_module_;
}

void ESMainWindowUI::HandleRemoveModule() {
    designer_scene->RemoveModule(del_module->data().toInt());
}

void ESMainWindowUI::ModuleConnected(int inputModuleId, int inputIndex, int outputModuleId,
                                     int outputIndex) {
    engine.Connect(inputModuleId, inputIndex, outputModuleId, outputIndex);
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
