#include "esmainwindow_ui.h"
#include <QApplication>
#include <QWheelEvent>
#include "esmodule_ui.h"
#include "ui_esmainwindow.h"

class QAction;
class QMenu;

QMenu* menu_view_;
QMenu* menu_module_;

QAction* add_module_2;
QAction* add_module_4;
QAction* del_module;

ESMainWindowUI::ESMainWindowUI(QWidget* parent) : QMainWindow(parent), ui_(new Ui::ESMainWindowUI) {
    ui_->setupUi(this);
    designer_view_ = new ESDesignerViewUI(ui_->frameDesigner);
    connect(designer_view_, &ESDesignerViewUI::MenuRequested, this, &ESMainWindowUI::MenuRequested);
    connect(designer_view_, SIGNAL(ModuleMenuRequested(int, int, ESModuleUI*)), this,
            SLOT(ModuleMenuRequested(int, int, ESModuleUI*)));

    menu_view_ = new QMenu(designer_view_);

    add_module_2 = new QAction("Add module 2x2", designer_view_);
    menu_view_->addAction(add_module_2);

    add_module_4 = new QAction("Add module 4x4", designer_view_);
    menu_view_->addAction(add_module_4);

    menu_module_ = new QMenu(designer_view_);
    del_module = new QAction("Del module", designer_view_);
    menu_module_->addAction(del_module);
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
}
