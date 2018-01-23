#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "esdesignerview_ui.h"

namespace Ui {
class ESMainWindowUI;
}

class ESModuleUI;
class ESMainWindowUI : public QMainWindow {
    Q_OBJECT

   public:
    explicit ESMainWindowUI(QWidget *parent = 0);
    ~ESMainWindowUI();

   private slots:
    void MenuRequested(int x, int y);
    void ModuleMenuRequested(int x, int y, ESModuleUI* module);

   private:
    void SetupActions();

    Ui::ESMainWindowUI *ui_;
    ESDesignerViewUI *designer_view_;
};

#endif  // MAINWINDOW_H
