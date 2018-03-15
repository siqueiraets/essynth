#include "ESMainWindowUI.h"
#include "ui_ESMainWindowUI.h"

#include <QApplication>
#include <QFileDialog>
#include <QGraphicsView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWheelEvent>

#include "ESConnectionUI.h"
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

static int currentTypeId = 0;
std::map<int, std::function<int(int x, int y)>> createModuleFnMap;

template <typename Type>
static void setupActionModuleType(QMenu* menu, QGraphicsView* view) {
    QAction* new_action;
    new_action =
        new QAction(QString("Add ") + QString::fromStdString(Type::GetModuleName()), nullptr);
    menu->addAction(new_action);

    auto connectFn = [typeId = currentTypeId, menu, new_action, view]() {
        auto point = new_action->data().toPointF();
        auto moduleId = engine.CreateModule<Type>();
        QString module_name = QString::fromStdString(Type::GetModuleName());
        ESModuleInfoUI moduleInfo{moduleId, Type::GetNumOutputs(), Type::GetNumInputs(), typeId,
                                  module_name};

        auto module = new ESModuleUI(menu_module_, menu_input_, moduleInfo);
        designer_scene->addItem(module);

        module->setPos(point.x(), point.y());
    };

    auto createModuleFn = [typeId = currentTypeId](int x, int y) {
        QString module_name = QString::fromStdString(Type::GetModuleName());
        auto moduleId = engine.CreateModule<Type>();
        ESModuleInfoUI moduleInfo{moduleId, Type::GetNumOutputs(), Type::GetNumInputs(), typeId,
                                  module_name};

        auto module = new ESModuleUI(menu_module_, menu_input_, moduleInfo);
        designer_scene->addItem(module);
        module->setPos(x, y);
        return moduleId;
    };
    createModuleFnMap[currentTypeId] = createModuleFn;

    QObject::connect(new_action, &QAction::triggered, new_action, connectFn);
    currentTypeId++;
};

template <typename... Types>
struct setupActionModules {
    void operator()(QMenu* menu, QGraphicsView* view) {
        int unused[]{(setupActionModuleType<Types>(menu, view), 0)...};
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

    setupActions();

    connect(designer_scene, SIGNAL(moduleConnected(ESModuleUI*, int, ESModuleUI*, int)), this,
            SLOT(moduleConnected(ESModuleUI*, int, ESModuleUI*, int)));
    connect(ui_->btnStart, SIGNAL(clicked()), this, SLOT(startAudio()));
    connect(ui_->btnStop, SIGNAL(clicked()), this, SLOT(stopAudio()));
    connect(ui_->actionOpen, SIGNAL(triggered()), this, SLOT(handleActionLoad()));
    connect(ui_->actionSave, SIGNAL(triggered()), this, SLOT(handleActionSave()));
    connect(del_module, SIGNAL(triggered()), this, SLOT(handleDelModule()));
    connect(add_const, SIGNAL(triggered()), this, SLOT(handleAddConst()));
    connect(del_const, SIGNAL(triggered()), this, SLOT(handleDelConst()));
}

ESMainWindowUI::~ESMainWindowUI() { delete ui_; }

void ESMainWindowUI::handleActionSave() {
    auto filename = QFileDialog::getSaveFileName(this, "Save Synth", "/", "Synth Files (*.ess)");
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly)) return;

    QJsonArray moduleArray;

    auto sceneItems = designer_scene->items();
    foreach (QGraphicsItem* item, sceneItems) {
        ESModuleUI* module = dynamic_cast<ESModuleUI*>(item);
        if (module != nullptr) {
            QJsonObject moduleObj;
            moduleObj["id"] = module->getId();
            moduleObj["type"] = module->getTypeId();
            moduleObj["x"] = module->pos().x();
            moduleObj["y"] = module->pos().y();
            moduleArray.push_back(moduleObj);
        }
    }

    QJsonArray constArray;
    foreach (QGraphicsItem* item, sceneItems) {
        ESConstValueUI* constValue = dynamic_cast<ESConstValueUI*>(item);

        if (constValue != nullptr) {
            QJsonObject constObj;
            auto info = constValue->getInfo();
            constObj["module"] = info.module;
            constObj["input"] = info.input;
            constObj["value"] = constValue->getTextValue();
            constObj["x"] = constValue->pos().x();
            constObj["y"] = constValue->pos().y();
            constArray.push_back(constObj);
        }
    }

    QJsonArray connectionArray;
    foreach (QGraphicsItem* item, sceneItems) {
        ESConnectionUI* connection = dynamic_cast<ESConnectionUI*>(item);
        if (connection != nullptr) {
            QJsonObject connectionObj;
            connectionObj["outputModule"] = connection->getOutputModule()->getId();
            connectionObj["outputIndex"] = connection->getOutputIndex();
            connectionObj["inputModule"] = connection->getInputModule()->getId();
            connectionObj["inputIndex"] = connection->getInputIndex();
            connectionObj["x"] = connection->pos().x();
            connectionObj["y"] = connection->pos().y();
            connectionArray.push_back(connectionObj);
        }
    }
    QJsonObject mainJsonObject;
    mainJsonObject["modules"] = moduleArray;
    mainJsonObject["constValues"] = constArray;
    mainJsonObject["connections"] = connectionArray;

    QJsonDocument jsonDoc(mainJsonObject);
    f.write(jsonDoc.toJson());

    // store data in f
    f.close();
}

void ESMainWindowUI::handleActionLoad() {
    auto filename = QFileDialog::getOpenFileName(this, "Open Synth", "/", "Synth Files (*.ess)");

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray saveData = f.readAll();
    std::map<int, int> idMap;

    QJsonDocument mainJsonObject(QJsonDocument::fromJson(saveData));
    QJsonArray moduleArray = mainJsonObject["modules"].toArray();
    foreach (QJsonValue moduleValue, moduleArray) {
        QJsonObject moduleObj = moduleValue.toObject();
        auto moduleId =
            addModule(moduleObj["type"].toInt(), moduleObj["x"].toInt(), moduleObj["y"].toInt());
        idMap[moduleObj["id"].toInt()] = moduleId;
    }

    QJsonArray constArray = mainJsonObject["constValues"].toArray();
    foreach (QJsonValue constValue, constArray) {
        QJsonObject constObj = constValue.toObject();
        auto moduleId = idMap[constObj["module"].toInt()];
        addConst(moduleId, constObj["input"].toInt(), constObj["value"].toString());
        setConstValue(moduleId, constObj["input"].toInt(), constObj["value"].toString());
    }

    QJsonArray connectionsArray = mainJsonObject["connections"].toArray();
    foreach (QJsonValue connectionValue, connectionsArray) {
        QJsonObject connectionObj = connectionValue.toObject();
        auto inputModule = idMap[connectionObj["inputModule"].toInt()];
        auto outputModule = idMap[connectionObj["outputModule"].toInt()];
        addConnection(outputModule, connectionObj["outputIndex"].toInt(), inputModule,
                      connectionObj["inputIndex"].toInt());
    }

    f.close();
}

void ESMainWindowUI::handleDelModule() {
    engine.DeleteModule(del_module->data().toInt());
    auto module = designer_scene->getModule(del_module->data().toInt());
    if (!module) return;
    designer_scene->removeItem(module);
    delete module;
}

void ESMainWindowUI::handleAddConst() {
    if (!add_const->data().canConvert<ESModuleInputInfoUI>()) {
        return;
    }

    ESModuleInputInfoUI info = add_const->data().value<ESModuleInputInfoUI>();
    addConst(info.module, info.input, "0");
}

void ESMainWindowUI::handleDelConst() {
    if (!del_const->data().canConvert<ESConstInfoUI>()) {
        return;
    }

    ESConstInfoUI info = del_const->data().value<ESConstInfoUI>();
    auto module = designer_scene->getModule(info.module);
    if (!module) return;

    auto constValue = module->getConst(info.input);
    delete constValue;
}

void ESMainWindowUI::moduleConnected(ESModuleUI* outputModule, int outputIndex,
                                     ESModuleUI* inputModule, int inputIndex) {
    engine.Connect(outputModule->getId(), outputIndex, inputModule->getId(), inputIndex);
}

void ESMainWindowUI::setupActions() {
    // Setup menu options to create modules
    setupActionModules<ESModuleAddFloat, ESModuleAddInt, ESModuleAudioOut, ESModuleCounter,
                       ESModuleDivideFloat, ESModuleDivideInt, ESModuleEventDivider,
                       ESModuleFloatToInt, ESModuleGate, ESModuleIntToFloat, ESModuleMidiEvent,
                       ESModuleMidiNote, ESModuleMultiplyFloat, ESModuleMultiplyInt,
                       ESModuleNoteToFreq, ESModuleSawOsc, ESModuleSineOsc, ESModuleSquareOsc,
                       ESModuleSubtractFloat, ESModuleSubtractInt, ESModuleTriangleOsc>()(
        menu_view_, designer_view_);
}

void ESMainWindowUI::startAudio() {
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

void ESMainWindowUI::stopAudio() {
    audioInterface.Stop();
    midiInterface.Stop();
}

void ESMainWindowUI::addConst(int module, int input, const QString& value) {
    auto modulePtr = designer_scene->getModule(module);
    if (!modulePtr) return;

    ESConstValueUI* constValue =
        new ESConstValueUI(ESConstInfoUI{module, input}, menu_const_, modulePtr, value);
    modulePtr->addConst(constValue);
    engine.SetConstData(module, input, ESData{.data_int32 = 0});

    constValue->registerSignal([this, constValue]() {
        auto constInfo = constValue->getInfo();
        setConstValue(constInfo.module, constInfo.input, constValue->getTextValue());
    });
}

void ESMainWindowUI::setConstValue(int module, int input, const QString& value) {
    bool ok = false;

    ESInt32Type intValue = value.toInt(&ok);
    if (ok) {
        engine.SetConstData(module, input, ESData{.data_int32 = intValue});
        return;
    }

    ESFloatType floatValue = value.toFloat(&ok);
    if (ok) {
        engine.SetConstData(module, input, ESData{.data_float = floatValue});
    }
}

int ESMainWindowUI::addModule(int typeId, int x, int y) {
    auto it = createModuleFnMap.find(typeId);
    if (it != createModuleFnMap.end()) {
        return createModuleFnMap[typeId](x, y);
    }

    return 0;
}

void ESMainWindowUI::addConnection(int outputModule, int outputIndex, int inputModule,
                                   int inputIndex) {
    ESModuleUI* outputModulePtr = designer_scene->getModule(outputModule);
    ESModuleUI* inputModulePtr = designer_scene->getModule(inputModule);
    if (!outputModulePtr || !inputModulePtr) {
        return;
    }

    designer_scene->addConnection(outputModulePtr, outputIndex, inputModulePtr, inputIndex);
    engine.Connect(outputModule, outputIndex, inputModule, inputIndex);
}
