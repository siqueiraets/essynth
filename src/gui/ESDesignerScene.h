#ifndef ESDESIGNERSCENE_UI_H
#define ESDESIGNERSCENE_UI_H

#include <QGraphicsScene>
#include <QString>

#include "ESModuleUI.h"

class QMenu;
class ESDesignerScene : public QGraphicsScene {
    Q_OBJECT
   public:
    ESDesignerScene(QMenu* contextMenu, QMenu* moduleMenu, QWidget* parent);
    virtual ~ESDesignerScene();

    void AddModule(int x, int y, const ESModuleInfoUI& moduleInfo);
    void RemoveModule(int moduleId);

   protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

   signals:
    void MenuRequested(int x, int y);
    void ModuleMenuRequested(int x, int y, ESModuleUI*);
    void ModuleConnected(int inputModuleId, int inputIndex, int outputModuleId, int outputIndex);

   private:
    QMenu* contextMenu_;
    QMenu* moduleMenu_;

    bool connecting_;
    int outputIndex_;
    ESModuleUI* itemConnectionOrig_;
    QPointF connectionOrigPos_;
    QGraphicsLineItem* currentConnection_;
};

#endif  // ESDESIGNERSCENE_UI_H
