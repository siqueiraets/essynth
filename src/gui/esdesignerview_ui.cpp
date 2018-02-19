#include "esdesignerview_ui.h"
#include <QApplication>
#include <QLayout>
#include <QMenu>
#include <QWheelEvent>
#include "esmodule_ui.h"

ESDesignerViewUI::ESDesignerViewUI(QWidget *parent)
    : QGraphicsView(parent), dragging_(false), item_drag_(nullptr) {
    auto layout = parent->layout();
    if (layout) {
        layout->addWidget(this);
    }
    scene_ = new QGraphicsScene(0, 0, 1024, 1024);
    setScene(scene_);
}

ESDesignerViewUI::~ESDesignerViewUI() {
    setScene(nullptr);
    delete scene_;
}

QPoint ESDesignerViewUI::GetLastPoint() { return last_point_; }

void ESDesignerViewUI::mousePressEvent(QMouseEvent *event) {
    auto button = event->button();
    switch (button) {
        case Qt::LeftButton:
            last_point_ = event->pos();
            item_drag_ = itemAt(event->pos());
            if (item_drag_) {
                button_down_ = true;
                drag_offset_ = mapToScene(last_point_) - item_drag_->sceneBoundingRect().topLeft();
            }
            break;
        case Qt::RightButton: {
            last_point_ = event->pos();
            auto item = dynamic_cast<ESModuleUI *>(itemAt(last_point_));
            if (!item) {
                emit MenuRequested(last_point_.x(), last_point_.y());
            } else {
                emit ModuleMenuRequested(last_point_.x(), last_point_.y(), item);
            }
        } break;
        default:
            event->ignore();
            break;
    }
}

void ESDesignerViewUI::mouseMoveEvent(QMouseEvent *event) {
    if (dragging_) {
        if (item_drag_) {
            ESModuleUI *module = dynamic_cast<ESModuleUI *>(item_drag_);
            if (!module) return;

            auto point = mapToScene(event->x(), event->y());
            point -= drag_offset_;
            module->setPos(point.x(), point.y());
        }
        return;
    }

    if (!button_down_ ||
        (QLineF(event->pos(), last_point_).length() < QApplication::startDragDistance())) {
        return;
    }

    if (item_drag_) {
        dragging_ = true;
    }
}

void ESDesignerViewUI::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    dragging_ = false;
    item_drag_ = nullptr;
    button_down_ = false;
}

void ESDesignerViewUI::wheelEvent(QWheelEvent *event) {
    // If ctrl is not pressed, do not handle the wheel event
    if (~QApplication::keyboardModifiers() & Qt::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    double factor = 1 + (event->angleDelta().y() / 1000.0f);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Execute the transformation ( Zoom in/out )
    scale(factor, factor);
    event->setAccepted(false);
}

ESModuleUI *ESDesignerViewUI::AddModule(int x, int y, int num_inputs, int num_outputs, QString name,
                                        const QVariant &userData) {
    auto module = new ESModuleUI(num_inputs, num_outputs, name, userData);

    // Translate widget coordinates to scene coordinates
    auto point = mapToScene(x, y);

    // Update module position
    module->setPos(point.x(), point.y());

    // Add module to scene
    scene_->addItem(module);

    return module;
}

void ESDesignerViewUI::RemoveModule(ESModuleUI *module) { scene_->removeItem(module); }
