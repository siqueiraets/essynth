#ifndef ESCONSTVALUETEXTUI_H
#define ESCONSTVALUETEXTUI_H

#include <QGraphicsTextItem>

class ESConstValueTextUI : public QGraphicsTextItem {
   public:
    ESConstValueTextUI(QGraphicsItem *parent);
    virtual ~ESConstValueTextUI() = default;

   protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusOutEvent(QFocusEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
   private:
    QString currentText_;

};

#endif  // ESCONSTVALUETEXTUI_H
