#include "ESDesignerScene.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QMenu>

#include "ESConnectionUI.h"
#include "ESModuleUI.h"

ESDesignerScene::ESDesignerScene(QMenu *contextMenu, QWidget *parent)
    : QGraphicsScene(0, 0, 1024, 1024, parent),
      contextMenu_(contextMenu),
      connecting_(false),
      outputIndex_(0),
      itemConnectionOrig_(nullptr),
      currentConnection_(nullptr) {}

ESDesignerScene::~ESDesignerScene() {}

void ESDesignerScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    auto button = event->button();
    if (button != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    auto item = dynamic_cast<ESModuleUI *>(itemAt(event->scenePos(), QTransform()));
    if (item) {
        auto point = item->mapFromScene(event->scenePos());
        if (item->isOutput(point.x(), point.y(), outputIndex_)) {
            connecting_ = true;
            itemConnectionOrig_ = item;
            connectionOrigPos_ = event->scenePos();
            event->ignore();
            return;
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void ESDesignerScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!connecting_) {
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }

    if (!currentConnection_) {
        currentConnection_ = new QGraphicsLineItem(connectionOrigPos_.x(), connectionOrigPos_.y(),
                                                   event->scenePos().x(), event->scenePos().y());
        addItem(currentConnection_);
    } else {
        currentConnection_->setLine(connectionOrigPos_.x(), connectionOrigPos_.y(),
                                    event->scenePos().x(), event->scenePos().y());
    }
}

void ESDesignerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (!connecting_) {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }

    if (!currentConnection_) {
        connecting_ = false;
        return;
    }

    removeItem(currentConnection_);
    currentConnection_ = nullptr;
    connecting_ = false;

    if (!itemConnectionOrig_) {
        return;
    }

    auto destItem = dynamic_cast<ESModuleUI *>(itemAt(event->scenePos(), QTransform()));
    if (!destItem || (destItem == itemConnectionOrig_)) {
        itemConnectionOrig_ = nullptr;
        return;
    }

    int inputIndex;
    auto point = destItem->mapFromScene(event->scenePos());
    if (destItem->isInput(point.x(), point.y(), inputIndex)) {
        ESConnectionUI *newConnection =
            new ESConnectionUI(outputIndex_, itemConnectionOrig_, inputIndex, destItem, 0);
        newConnection->updateGeometry();
        addItem(newConnection);
        emit ModuleConnected(itemConnectionOrig_, outputIndex_, destItem, inputIndex);
    }

    itemConnectionOrig_ = nullptr;
}

void ESDesignerScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    if (nullptr != itemAt(event->scenePos(), QTransform())) {
        QGraphicsScene::contextMenuEvent(event);
        return;
    }
    contextMenu_->exec(event->screenPos());
}

void ESDesignerScene::wheelEvent(QGraphicsSceneWheelEvent *event) {
    // If ctrl is not pressed, do not handle the wheel event
    if (~QApplication::keyboardModifiers() & Qt::ControlModifier) {
        QGraphicsScene::wheelEvent(event);
        return;
    }

    double factor = 1 + (event->delta() / 1000.0f);
    for (auto &view : views()) {
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Execute the transformation ( Zoom in/out )
        view->scale(factor, factor);
    }

    event->setAccepted(false);
}

ESModuleUI *ESDesignerScene::getModule(int moduleId) {
    foreach (QGraphicsItem *item, items()) {
        ESModuleUI *moduleItem = dynamic_cast<ESModuleUI *>(item);
        if (!moduleItem) continue;
        if (moduleItem->getId() == moduleId) {
            return moduleItem;
        }
    }

    return nullptr;
}

