#pragma once

#include "shape.h"

#include <QVector>
#include <QWidget>

class WidgetCanvas : public QWidget {
public:
    explicit WidgetCanvas(QWidget *parent = nullptr);

    void addShape(ShapeType type);
    void removeActiveShape();
    void clearShapes();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVector<Shape> shapes;
    int activeIndex = -1;
    bool dragging = false;
    QPointF dragOffset;
};
