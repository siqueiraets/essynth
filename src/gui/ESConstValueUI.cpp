#include "ESConstValueUI.h"
#include "ESModuleUI.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

static const int ModuleHeaderHeight = 20;
static const int ModuleFirstPortOffset = 5;
static const int ModulePortHeight = 20;
static const int ModuleLastPortOffset = 5;
static const int ModulePortSize = 8;
static const int ModuleCaptionSize = 13;
static const int ModulePenWidth = 2;
static const QColor ModuleColor(32, 28, 53);
static const QColor ModuleBackgroundColor(122, 122, 122);
static const QColor ModuleOutputColor(32, 28, 53);

ESConstValueUI::ESConstValueUI(const ESConstInfoUI &inputInfo, QMenu *menu, ESModuleUI *parent)
    : QGraphicsItem(parent), inputInfo_(inputInfo), value_(0), menu_(menu) {
    textItem_ = new ESConstValueTextUI(this);
    textItem_->setPos(0, 0);
    textItem_->setPlainText("0");

    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIsMovable, false);  // TODO is not movable for now
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF ESConstValueUI::boundingRect() const { return QRectF(0, 0, 60, 20); }

void ESConstValueUI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                           QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(ModuleColor, ModulePenWidth));
    painter->setBrush(ModuleBackgroundColor);
    painter->drawRect(0, 0, 60, 20);

    painter->setPen(QPen(ModuleColor, 1));
    painter->setBrush(ModuleOutputColor);
    painter->drawRect(52, 6, 8, 8);
}

QVariant ESConstValueUI::getValue() { return value_; }

QString ESConstValueUI::getTextValue() { return textItem_->toPlainText(); }

ESConstInfoUI ESConstValueUI::getInfo() { return inputInfo_; }

void ESConstValueUI::setValue(const QVariant &value) { value_ = value; }

void ESConstValueUI::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    foreach (QAction *action, menu_->actions()) {
        action->setData(QVariant::fromValue(inputInfo_));
    }

    menu_->exec(event->screenPos());
}

QVariant ESConstValueUI::itemChange(GraphicsItemChange change, const QVariant &value) {
    // if (change == QGraphicsItem::ItemPositionChange) {
    // for (auto &connection : connections_) {
    // connection->updateGeometry();
    //}
    //}

    return value;
}
