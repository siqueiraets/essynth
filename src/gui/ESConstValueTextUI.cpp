#include "ESConstValueTextUI.h"

#include <QKeyEvent>

ESConstValueTextUI::ESConstValueTextUI(QGraphicsItem *parent) : QGraphicsTextItem(parent) {
    setFlag(QGraphicsItem::ItemIsSelectable);
}

QVariant ESConstValueTextUI::itemChange(GraphicsItemChange change, const QVariant &value) {
    // if (change == QGraphicsItem::ItemSelectedHasChanged)
    // emit selectedChange(this);
     return value;
}

void ESConstValueTextUI::focusOutEvent(QFocusEvent *event) {
    setTextInteractionFlags(Qt::NoTextInteraction);
    // emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void ESConstValueTextUI::keyPressEvent(QKeyEvent *event)
{
    if(event->text() == "\n") {
        event->ignore();
        return;
    }

    QGraphicsTextItem::keyPressEvent(event);
}

void ESConstValueTextUI::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (textInteractionFlags() == Qt::NoTextInteraction) {
        setTextInteractionFlags(Qt::TextEditorInteraction);
    }
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}
