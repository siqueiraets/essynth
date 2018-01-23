#ifndef ESMODULE_UI_H
#define ESMODULE_UI_H

#include <QGraphicsItem>

#define MODULE_WIDTH 100
#define MODULE_HEADER_HEIGHT 20
#define MODULE_FIRST_PORT_OFFSET 5
#define MODULE_PORT_HEIGHT 20
#define MODULE_LAST_PORT_OFFSET 5
#define MODULE_PORT_SIZE 8
#define MODULE_CAPTION_SIZE 13
#define MODULE_PEN_WIDTH 2
#define MODULE_COLOR_1 QColor(32, 28, 53)
#define MODULE_COLOR_BACKGROUND QColor(122, 122, 122)
#define MODULE_COLOR_INPUT QColor(113, 0, 0)
#define MODULE_COLOR_OUTPUT QColor(0, 74, 0)

class ESModuleUI : public QGraphicsItem {
   public:
    ESModuleUI(int num_inputs, int num_outputs, QString name);
    virtual ~ESModuleUI() = default;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

   private:
    int width_;
    int height_;
    int num_inputs_;
    int num_outputs_;
    QString name_;
};

#endif  // ESMODULE_UI_H
