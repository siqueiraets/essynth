#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ESMainWindowUI;
}

class QGraphicsView;
class ESModuleUI;
class ESMainWindowUI : public QMainWindow {
    Q_OBJECT

   public:
    explicit ESMainWindowUI(QWidget *parent = 0);
    ~ESMainWindowUI();

   private slots:
    void HandleDelModule();
    void HandleAddConst();
    void HandleDelConst();
    void ModuleConnected(ESModuleUI *inputModule, int inputIndex, ESModuleUI *outputModule,
                         int outputIndex);
    void StartAudio();
    void StopAudio();

   private:
    void SetupActions();

    Ui::ESMainWindowUI *ui_;
    QGraphicsView *designer_view_;
};

#endif  // MAINWINDOW_H
