#include "ESMainWindowUI.h"
#include "ui_ESMainWindowUI.h"

#include <QApplication>
#include <QGraphicsView>
#include <QWheelEvent>

#include "ESConstValueUI.h"
#include "ESDesignerScene.h"
#include "ESModule.h"

#include "ESEngine.h"
#include "ESIoPortAudio.h"
#include "ESIoPortMidi.h"
#include "ESModuleAddFloat.h"
#include "ESModuleAddInt.h"
#include "ESModuleAudioOut.h"
#include "ESModuleCounter.h"
#include "ESModuleDivideFloat.h"
#include "ESModuleDivideInt.h"
#include "ESModuleEventDivider.h"
#include "ESModuleFloatToInt.h"
#include "ESModuleGate.h"
#include "ESModuleIntToFloat.h"
#include "ESModuleMidiEvent.h"
#include "ESModuleMidiNote.h"
#include "ESModuleMultiplyFloat.h"
#include "ESModuleMultiplyInt.h"
#include "ESModuleNoteToFreq.h"
#include "ESModuleSawOsc.h"
#include "ESModuleSineOsc.h"
#include "ESModuleSquareOsc.h"
#include "ESModuleSubtractFloat.h"
#include "ESModuleSubtractInt.h"
#include "ESModuleTriangleOsc.h"

#include <iostream>

using namespace ESSynth;
ESEngine engine;
ESIoPortAudio audioInterface(&engine);
ESIoPortMidi midiInterface;

class QAction;
class QMenu;

QMenu* menu_view_;
QMenu* menu_module_;
QMenu* menu_input_;
QMenu* menu_const_;

QAction* del_module;
QAction* add_const;
QAction* del_const;
ESDesignerScene* designer_scene;

template <typename Type>
static void SetupActionModuleType(QMenu* menu, QGraphicsView* view) {
    QAction* new_action;
    QString module_name = QString::fromStdString(Type::GetModuleName());
    new_action = new QAction(QString("Add ") + module_name, nullptr);
    menu->addAction(new_action);

    auto connectFn = [menu, module_name, view]() {
        auto moduleId = engine.CreateModule<Type>();
        auto scenePoint = view->mapToScene(view->mapFromGlobal(menu->pos()));
        ESModuleInfoUI moduleInfo{moduleId, Type::GetNumOutputs(), Type::GetNumInputs(),
                                  module_name};

        auto module = new ESModuleUI(menu_module_, menu_input_, moduleInfo);
        designer_scene->addItem(module);

        module->setPos(scenePoint.x(), scenePoint.y());
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
    menu_input_ = new QMenu(this);
    menu_const_ = new QMenu(this);

    designer_view_ = new QGraphicsView(this);
    designer_scene = new ESDesignerScene(menu_view_, this);
    add_const = new QAction("Add Const", this);
    del_module = new QAction("Del Module", this);
    del_const = new QAction("Del Const", this);

    designer_view_->setScene(designer_scene);
    menu_module_->addAction(del_module);
    menu_input_->addAction(add_const);
    menu_const_->addAction(del_const);

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

    connect(designer_scene, SIGNAL(ModuleConnected(ESModuleUI*, int, ESModuleUI*, int)), this,
            SLOT(ModuleConnected(ESModuleUI*, int, ESModuleUI*, int)));
    connect(ui_->btnStart, SIGNAL(clicked()), this, SLOT(StartAudio()));
    connect(ui_->btnStop, SIGNAL(clicked()), this, SLOT(StopAudio()));
    connect(del_module, SIGNAL(triggered()), this, SLOT(HandleDelModule()));
    connect(add_const, SIGNAL(triggered()), this, SLOT(HandleAddConst()));
    connect(del_const, SIGNAL(triggered()), this, SLOT(HandleDelConst()));
}

ESMainWindowUI::~ESMainWindowUI() { delete ui_; }

void ESMainWindowUI::HandleDelModule() {
    engine.DeleteModule(del_module->data().toInt());
    auto module = designer_scene->getModule(del_module->data().toInt());
    if (!module) return;
    designer_scene->removeItem(module);
    delete module;
}

void ESMainWindowUI::HandleAddConst() {
    if (!add_const->data().canConvert<ESModuleInputInfoUI>()) {
        return;
    }

    ESModuleInputInfoUI info = add_const->data().value<ESModuleInputInfoUI>();
    auto module = designer_scene->getModule(info.module);
    if (!module) return;

    ESConstValueUI* constValue =
        new ESConstValueUI(ESConstInfoUI{info.module, info.input}, menu_const_, module);
    module->addConst(constValue);
    constValue->registerSignal([constValue]() {
        auto constInfo = constValue->getInfo();
        bool ok = false;

        ESInt32Type intValue = constValue->getTextValue().toInt(&ok);
        if (ok) {
            engine.SetConstData(constInfo.module, constInfo.input, ESData{.data_int32 = intValue});
            return;
        }

        ESFloatType floatValue = constValue->getTextValue().toFloat(&ok);
        if (ok) {
            engine.SetConstData(constInfo.module, constInfo.input,
                                ESData{.data_float = floatValue});
        }
    });
}

void ESMainWindowUI::HandleDelConst() {
    if (!del_const->data().canConvert<ESConstInfoUI>()) {
        return;
    }

    ESConstInfoUI info = del_const->data().value<ESConstInfoUI>();
    auto module = designer_scene->getModule(info.module);
    if (!module) return;

    auto constValue = module->getConst(info.input);
    delete constValue;
}

void ESMainWindowUI::ModuleConnected(ESModuleUI* outputModule, int outputIndex,
                                     ESModuleUI* inputModule, int inputIndex) {
    engine.Connect(outputModule->getId(), outputIndex, inputModule->getId(), inputIndex);
}

void ESMainWindowUI::SetupActions() {
    // Setup menu options to create modules
    SetupActionModules<ESModuleAddFloat, ESModuleAddInt, ESModuleAudioOut, ESModuleCounter,
                       ESModuleDivideFloat, ESModuleDivideInt, ESModuleEventDivider,
                       ESModuleFloatToInt, ESModuleGate, ESModuleIntToFloat, ESModuleMidiEvent,
                       ESModuleMidiNote, ESModuleMultiplyFloat, ESModuleMultiplyInt,
                       ESModuleNoteToFreq, ESModuleSawOsc, ESModuleSineOsc, ESModuleSquareOsc,
                       ESModuleSubtractFloat, ESModuleSubtractInt, ESModuleTriangleOsc>()(
        menu_view_, designer_view_);
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
