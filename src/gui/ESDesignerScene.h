#ifndef ESDESIGNERSCENE_UI_H
#define ESDESIGNERSCENE_UI_H

#include <QGraphicsScene>
#include <QString>

#include "ESModuleUI.h"

class QMenu;
class ESDesignerScene : public QGraphicsScene {
    Q_OBJECT
   public:
    ESDesignerScene(QMenu* contextMenu, QWidget* parent);
    virtual ~ESDesignerScene();

    ESModuleUI* getModule(int moduleId);

   protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

   signals:
    void ModuleConnected(ESModuleUI* outputModule, int outputIndex, ESModuleUI* inputModule,
                         int inputIndex);

   private:
    QMenu* contextMenu_;

    bool connecting_;
    int outputIndex_;
    ESModuleUI* itemConnectionOrig_;
    QPointF connectionOrigPos_;
    QGraphicsLineItem* currentConnection_;
};

#endif  // ESDESIGNERSCENE_UI_H
