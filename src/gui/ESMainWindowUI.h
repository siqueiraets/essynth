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
    void handleActionSave();
    void handleActionLoad();
    void handleDelModule();
    void handleAddConst();
    void handleDelConst();
    void moduleConnected(ESModuleUI *outputModule, int outputIndex, ESModuleUI *inputModule,
                         int inputIndex);
    void startAudio();
    void stopAudio();

    int addModule(int typeId, int x, int y);
    void addConst(int module, int input, const QString& value);
    void setConstValue(int module, int input, const QString &value);
    void addConnection(int outputModule, int outputIndex, int inputModule, int inputIndex);

   private:
    void setupActions();

    Ui::ESMainWindowUI *ui_;
    QGraphicsView *designer_view_;
};

#endif  // MAINWINDOW_H
