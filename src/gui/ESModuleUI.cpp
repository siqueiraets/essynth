#include "ESModuleUI.h"
#include "ESConnectionUI.h"
#include "ESConstValueUI.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

static const int ModuleWidth = 100;
static const int ModuleHeaderHeight = 20;
static const int ModuleFirstPortOffset = 5;
static const int ModulePortHeight = 20;
static const int ModuleLastPortOffset = 5;
static const int ModulePortSize = 8;
static const int ModuleCaptionSize = 13;
static const int ModulePenWidth = 2;
static const QColor ModuleColor(32, 28, 53);
static const QColor ModuleSelectedColor(82, 105, 142);
static const QColor ModuleBackgroundColor(122, 122, 122);
static const QColor ModuleInputColor(133, 0, 0);
static const QColor ModuleOutputColor(32, 28, 53);

constexpr int getPortY(int port) {
    return ModuleHeaderHeight + ModuleFirstPortOffset + port * ModulePortHeight +
           ModulePortHeight / 2 - ModulePortSize / 2;
}

constexpr int getModuleHeight(int num_inputs, int num_outputs) {
    return ModuleHeaderHeight + ModuleFirstPortOffset +
           ((num_inputs > num_outputs) ? num_inputs : num_outputs) * ModulePortHeight +
           ModuleLastPortOffset;
}

ESModuleUI::ESModuleUI(QMenu *contextMenu, QMenu *inputMenu, const ESModuleInfoUI &moduleInfo)
    : contextMenu_(contextMenu), inputMenu_(inputMenu), moduleInfo_(moduleInfo) {
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

ESModuleUI::~ESModuleUI() {
    while (connections_.size()) {
        auto connection = connections_.front();
        connections_.erase(connections_.begin());
        delete connection;
    }
}

QRectF ESModuleUI::boundingRect() const {
    return QRectF(
        0 - ModulePenWidth / 2, 0 - ModulePenWidth / 2, ModuleWidth + ModulePenWidth / 2,
        getModuleHeight(moduleInfo_.numInputs, moduleInfo_.numOutputs) + ModulePenWidth / 2);
}

void ESModuleUI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(ModuleColor, ModulePenWidth));
    painter->setBrush(ModuleColor);
    painter->drawRect(0, 0, ModuleWidth, ModuleHeaderHeight);

    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(ModuleSelectedColor, ModulePenWidth));
    } else {
        painter->setPen(QPen(ModuleColor, ModulePenWidth));
    }
    painter->setBrush(ModuleBackgroundColor);
    painter->drawRect(
        0, ModuleHeaderHeight, ModuleWidth,
        ModuleFirstPortOffset +
            ((moduleInfo_.numInputs > moduleInfo_.numOutputs) ? moduleInfo_.numInputs
                                                              : moduleInfo_.numOutputs) *
                ModulePortHeight +
            ModuleLastPortOffset);

    painter->setPen(QPen(Qt::white, ModulePenWidth));
    QFont font = painter->font();
    font.setPixelSize(ModuleCaptionSize);
    painter->setFont(font);
    painter->drawText(5, ModuleHeaderHeight - 5, moduleInfo_.name);

    for (int i = 0; i < moduleInfo_.numInputs; ++i) {
        painter->setPen(QPen(Qt::black, 1));
        painter->setBrush(ModuleInputColor);
        painter->drawRect(0, getPortY(i), ModulePortSize, ModulePortSize);
    }

    for (int i = 0; i < moduleInfo_.numOutputs; ++i) {
        painter->setPen(QPen(ModuleColor, 1));
        painter->setBrush(ModuleOutputColor);
        painter->drawRect(ModuleWidth - ModulePortSize, getPortY(i), ModulePortSize,
                          ModulePortSize);
    }
}

bool ESModuleUI::isInput(int x, int y, int &index) {
    for (int i = 0; i < moduleInfo_.numInputs; ++i) {
        if ((x >= 0) && (x <= ModulePortSize) && (y >= getPortY(i)) &&
            (y <= (getPortY(i) + ModulePortSize))) {
            index = i;
            return true;
        }
    }
    return false;
}

bool ESModuleUI::isOutput(int x, int y, int &index) {
    for (int i = 0; i < moduleInfo_.numOutputs; ++i) {
        if ((x >= ModuleWidth - ModulePortSize) && (x <= ModuleWidth) && (y >= getPortY(i)) &&
            (y <= (getPortY(i) + ModulePortSize))) {
            index = i;
            return true;
        }
    }
    return false;
}

QPoint ESModuleUI::getInputCoordinates(int input) const {
    return QPoint(ModulePortSize / 2, getPortY(input) + ModulePortSize / 2);
}

QPoint ESModuleUI::getOutputCoordinates(int output) const {
    return QPoint(ModuleWidth - ModulePortSize / 2, getPortY(output) + ModulePortSize / 2);
}

void ESModuleUI::addConnection(ESConnectionUI *connection) {
    removeConnection(connection);
    connections_.push_back(connection);
}

void ESModuleUI::removeConnection(ESConnectionUI *connection) {
    auto it = connections_.begin();
    while (it != connections_.end()) {
        if (*it == connection) {
            it = connections_.erase(it);
        } else {
            ++it;
        }
    }
}

void ESModuleUI::addConst(ESConstValueUI *constValue) {
    constValue->setParentItem(this);
    constValue->setPos(-constValue->boundingRect().width(),
                       getPortY(constValue->getInfo().input) - 6);
}

ESConstValueUI *ESModuleUI::getConst(int input) {
    foreach (QGraphicsItem *item, childItems()) {
        ESConstValueUI *constValue = dynamic_cast<ESConstValueUI *>(item);
        if (constValue && constValue->getInfo().input == input) {
            return constValue;
        }
    }
    return nullptr;
}

int ESModuleUI::getId() const { return moduleInfo_.id; }

int ESModuleUI::getTypeId() const { return moduleInfo_.typeId; }

void ESModuleUI::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    int index;
    auto itemPos = event->pos();
    if (isInput(itemPos.x(), itemPos.y(), index)) {
        foreach (QAction *action, inputMenu_->actions()) {
            action->setData(QVariant::fromValue(ESModuleInputInfoUI{moduleInfo_.id, index}));
        }
        inputMenu_->exec(event->screenPos());
    } else {
        foreach (QAction *action, contextMenu_->actions()) { action->setData(moduleInfo_.id); }
        contextMenu_->exec(event->screenPos());
    }
}

QVariant ESModuleUI::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        for (auto &connection : connections_) {
            connection->updateGeometry();
        }
    }

    return value;
}
