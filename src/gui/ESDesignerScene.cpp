#include "ESDesignerScene.h"

#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsView>
#include <QMenu>

#include "ESConnectionUI.h"
#include "ESModuleUI.h"

ESDesignerScene::ESDesignerScene(QMenu *contextMenu, QMenu *moduleMenu, QWidget *parent)
    : QGraphicsScene(0, 0, 1024, 1024, parent),
      contextMenu_(contextMenu),
      moduleMenu_(moduleMenu),
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
        outputIndex_ = item->isOutput(point.x(), point.y());
        if (outputIndex_ != -1) {
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

    auto point = destItem->mapFromScene(event->scenePos());
    int inputIndex = destItem->isInput(point.x(), point.y());
    if (inputIndex != -1) {
        ESConnectionUI *newConnection =
            new ESConnectionUI(outputIndex_, itemConnectionOrig_, inputIndex, destItem, 0);
        newConnection->updateGeometry();
        addItem(newConnection);
        emit ModuleConnected(itemConnectionOrig_->getModuleId(), outputIndex_,
                             destItem->getModuleId(), inputIndex);
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

void ESDesignerScene::AddModule(int x, int y, const ESModuleInfoUI &moduleInfo) {
    auto module = new ESModuleUI(moduleMenu_, moduleInfo);

    // Add module to scene
    addItem(module);

    // Update module position
    module->setPos(x, y);
}

void ESDesignerScene::RemoveModule(int moduleId) {
    foreach (QGraphicsItem *item, items()) {
        ESModuleUI *moduleItem = dynamic_cast<ESModuleUI *>(item);
        if (moduleItem->getModuleId() == moduleId) {
            removeItem(moduleItem);
        }
    }
}
