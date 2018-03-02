#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ESMainWindowUI;
}

class QGraphicsView;
class ESMainWindowUI : public QMainWindow {
    Q_OBJECT

   public:
    explicit ESMainWindowUI(QWidget *parent = 0);
    ~ESMainWindowUI();

   private slots:
    void HandleRemoveModule();
    void ModuleConnected(int inputModuleId, int inputIndex, int outputModuleId, int outputIndex);
    void StartAudio();
    void StopAudio();

   private:
    void SetupActions();

    Ui::ESMainWindowUI *ui_;
    QGraphicsView *designer_view_;
};

#endif  // MAINWINDOW_H
