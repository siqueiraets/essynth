#include "esmodule_ui.h"
#include <QGraphicsScene>
#include <QPainter>

constexpr int getPortY(int port) {
    return MODULE_HEADER_HEIGHT + MODULE_FIRST_PORT_OFFSET + port * MODULE_PORT_HEIGHT +
           MODULE_PORT_HEIGHT / 2 - MODULE_PORT_SIZE / 2;
}

constexpr int getModuleHeight(int num_inputs, int num_outputs) {
    return MODULE_HEADER_HEIGHT + MODULE_FIRST_PORT_OFFSET +
           ((num_inputs > num_outputs) ? num_inputs : num_outputs) * MODULE_PORT_HEIGHT +
           MODULE_LAST_PORT_OFFSET;
}

ESModuleUI::ESModuleUI(int num_inputs, int num_outputs, QString name, const QVariant &userData)
    : num_inputs_(num_inputs), num_outputs_(num_outputs), name_(name), userData_(userData) {
    height_ = getModuleHeight(num_inputs_, num_outputs_);
    width_ = MODULE_WIDTH;
}

QRectF ESModuleUI::boundingRect() const {
    return QRectF(0 - MODULE_PEN_WIDTH / 2, 0 - MODULE_PEN_WIDTH / 2, width_ + MODULE_PEN_WIDTH / 2,
                  height_ + MODULE_PEN_WIDTH / 2);
}

void ESModuleUI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(MODULE_COLOR_1, MODULE_PEN_WIDTH));
    painter->setBrush(MODULE_COLOR_1);
    painter->drawRect(0, 0, width_, MODULE_HEADER_HEIGHT);

    painter->setPen(QPen(MODULE_COLOR_1, MODULE_PEN_WIDTH));
    painter->setBrush(MODULE_COLOR_BACKGROUND);
    painter->drawRect(
        0, MODULE_HEADER_HEIGHT, width_,
        MODULE_FIRST_PORT_OFFSET +
            ((num_inputs_ > num_outputs_) ? num_inputs_ : num_outputs_) * MODULE_PORT_HEIGHT +
            MODULE_LAST_PORT_OFFSET);

    painter->setPen(QPen(Qt::white, MODULE_PEN_WIDTH));
    QFont font = painter->font();
    font.setPixelSize(MODULE_CAPTION_SIZE);
    painter->setFont(font);
    painter->drawText(5, MODULE_HEADER_HEIGHT - 5, name_);

    for (int i = 0; i < num_inputs_; ++i) {
        painter->setPen(QPen(Qt::black, 1));
        painter->setBrush(MODULE_COLOR_INPUT);
        painter->drawRect(0, getPortY(i), MODULE_PORT_SIZE, MODULE_PORT_SIZE);
    }

    for (int i = 0; i < num_outputs_; ++i) {
        painter->setPen(QPen(MODULE_COLOR_1, 1));
        painter->setBrush(MODULE_COLOR_OUTPUT);
        painter->drawRect(MODULE_WIDTH - MODULE_PORT_SIZE, getPortY(i), MODULE_PORT_SIZE,
                          MODULE_PORT_SIZE);
    }
}
