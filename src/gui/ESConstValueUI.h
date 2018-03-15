#ifndef ESCONSTVALUEUI_H
#define ESCONSTVALUEUI_H

#include <QGraphicsItem>
#include <QObject>
#include <QTextDocument>
#include <QVariant>

#include "ESConstValueTextUI.h"

typedef struct {
    int module;
    int input;
} ESConstInfoUI;
Q_DECLARE_METATYPE(ESConstInfoUI);

class QMenu;
class ESModuleUI;
class ESConstValueTextUI;
class ESConstValueUI : public QGraphicsItem {
   public:
    ESConstValueUI(const ESConstInfoUI &inputInfo, QMenu *menu, ESModuleUI *parent,
                   const QString &value);
    virtual ~ESConstValueUI() = default;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    ESConstInfoUI getInfo();

    QVariant getValue();
    void setValue(const QVariant &value);

    QString getTextValue();

    template <typename Oper>
    void registerSignal(Oper oper) {
        QObject::connect(textItem_->document(), &QTextDocument::contentsChanged,
                         textItem_->document(), oper);
    }

   protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

   private:
    ESConstInfoUI inputInfo_;
    ESModuleUI *parent_;
    ESConstValueTextUI *textItem_;
    QVariant value_;
    QMenu *menu_;
};

#endif  // ESCONSTVALUEUI_H
