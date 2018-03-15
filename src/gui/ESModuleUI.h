#ifndef ESMODULE_UI_H
#define ESMODULE_UI_H

#include <QGraphicsItem>
#include <QVariant>

typedef struct {
    int id;
    int numOutputs;
    int numInputs;
    int typeId;
    QString name;
} ESModuleInfoUI;
Q_DECLARE_METATYPE(ESModuleInfoUI);

typedef struct {
    int module;
    int input;
} ESModuleInputInfoUI;
Q_DECLARE_METATYPE(ESModuleInputInfoUI);

class ESConnectionUI;
class ESConstValueUI;
class QMenu;
class ESModuleUI : public QGraphicsItem {
   public:
    ESModuleUI(QMenu *contextMenu, QMenu *inputMenu, const ESModuleInfoUI &moduleInfo);
    virtual ~ESModuleUI();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    bool isInput(int x, int y, int &index);
    bool isOutput(int x, int y, int &index);
    QPoint getInputCoordinates(int input) const;
    QPoint getOutputCoordinates(int output) const;

    void addConnection(ESConnectionUI *connection);
    void removeConnection(ESConnectionUI *connection);

    void addConst(ESConstValueUI *constValue);
    ESConstValueUI *getConst(int input);

    int getId() const;
    int getTypeId() const;

   protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

   private:
    QMenu *contextMenu_;
    QMenu *inputMenu_;
    ESModuleInfoUI moduleInfo_;
    QVariant userData_;
    std::list<ESConnectionUI *> connections_;
};

#endif  // ESMODULE_UI_H
