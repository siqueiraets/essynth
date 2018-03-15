#include "ESConnectionUI.h"
#include "ESModuleUI.h"

ESConnectionUI::ESConnectionUI(int outputIndex, ESModuleUI* outputModule, int inputIndex,
                               ESModuleUI* inputModule, const QVariant& userData)
    : outputIndex_(outputIndex),
      outputModule_(outputModule),
      inputIndex_(inputIndex),
      inputModule_(inputModule),
      userData_(userData) {
    updateGeometry();
    inputModule_->addConnection(this);
    outputModule_->addConnection(this);
}

ESConnectionUI::~ESConnectionUI() {
    outputModule_->removeConnection(this);
    inputModule_->removeConnection(this);
}

void ESConnectionUI::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                           QWidget* widget) {
    QGraphicsLineItem::paint(painter, option, widget);
}

void ESConnectionUI::updateGeometry() {
    auto origPoint = outputModule_->getOutputCoordinates(outputIndex_);
    auto destPoint = inputModule_->getInputCoordinates(inputIndex_);

    QLineF line(mapFromItem(outputModule_, origPoint.x(), origPoint.y()),
                mapFromItem(inputModule_, destPoint.x(), destPoint.y()));
    QGraphicsLineItem::setLine(line);
}

ESModuleUI* ESConnectionUI::getInputModule() { return inputModule_; }

ESModuleUI* ESConnectionUI::getOutputModule() { return outputModule_; }

int ESConnectionUI::getInputIndex() { return inputIndex_; }

int ESConnectionUI::getOutputIndex() { return outputIndex_; }

