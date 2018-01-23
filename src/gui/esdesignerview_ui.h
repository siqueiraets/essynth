#ifndef ESDESIGNERVIEW_UI_H
#define ESDESIGNERVIEW_UI_H

#include <QGraphicsView>

class ESModuleUI;
class ESDesignerViewUI : public QGraphicsView {
    Q_OBJECT
   public:
    explicit ESDesignerViewUI(QWidget* parent);
    virtual ~ESDesignerViewUI();

    ESModuleUI *AddModule(int x, int y, int num_inputs, int num_outputs, QString name);
    void RemoveModule(ESModuleUI* module);

   protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

   signals:
    void MenuRequested(int x, int y);
    void ModuleMenuRequested(int x, int y, ESModuleUI*);

   private:
    QGraphicsScene* scene_;

    bool button_down_;
    bool dragging_;
    QGraphicsItem* item_drag_;
    QPoint last_point_;
    QPointF drag_offset_;
};

#endif  // ESDESIGNERVIEW_UI_H
