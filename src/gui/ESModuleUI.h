#ifndef ESMODULE_UI_H
#define ESMODULE_UI_H

#include <QGraphicsItem>
#include <QVariant>

typedef struct {
    int id;
    int numOutputs;
    int numInputs;
    QString name;
} ESModuleInfoUI;

class ESConnectionUI;
class QMenu;
class ESModuleUI : public QGraphicsItem {
   public:
    ESModuleUI(QMenu *contextMenu, const ESModuleInfoUI &moduleInfo);
    virtual ~ESModuleUI();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int isInput(int x, int y);
    int isOutput(int x, int y);
    QPoint getInputCoordinates(int input) const;
    QPoint getOutputCoordinates(int output) const;

    void addConnection(ESConnectionUI *connection);
    void removeConnection(ESConnectionUI *connection);

    int getModuleId() const;

   protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

   private:
    QMenu *contextMenu_;
    ESModuleInfoUI moduleInfo_;
    QVariant userData_;
    std::list<ESConnectionUI *> connections_;
};

#endif  // ESMODULE_UI_H
