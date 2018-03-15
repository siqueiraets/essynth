#ifndef ESCONNECTION_UI_H
#define ESCONNECTION_UI_H

#include <QGraphicsLineItem>
#include <QVariant>

class ESModuleUI;
class ESConnectionUI : public QGraphicsLineItem {
   public:
    ESConnectionUI(int outputIndex, ESModuleUI* outputModule, int inputIndex,
                   ESModuleUI* inputModule, const QVariant& userData);
    virtual ~ESConnectionUI();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void updateGeometry();

    ESModuleUI* getInputModule();
    ESModuleUI* getOutputModule();
    int getInputIndex();
    int getOutputIndex();

   private:
    int outputIndex_;
    ESModuleUI* outputModule_;

    int inputIndex_;
    ESModuleUI* inputModule_;

    QVariant userData_;
};

#endif  // ESCONNECTION_UI_H
