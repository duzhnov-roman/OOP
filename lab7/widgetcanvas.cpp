#include "widgetcanvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPolygonF>

WidgetCanvas::WidgetCanvas(QWidget *parent) : QWidget(parent) {
    setMinimumSize(640, 420);
}

void WidgetCanvas::addShape(ShapeType type) {
    const int offset = (shapes.size() % 6) * 35;
    shapes.append(Shape{type, QRectF(60 + offset, 60 + offset, 140, 100)});
    activeIndex = shapes.size() - 1;
    dragging = false;
    update();
}

void WidgetCanvas::removeActiveShape() {
    if (activeIndex == -1)
        return;

    shapes.removeAt(activeIndex);
    activeIndex = -1;
    dragging = false;
    update();
}

void WidgetCanvas::clearShapes() {
    shapes.clear();
    activeIndex = -1;
    dragging = false;
    update();
}

void WidgetCanvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    for (int i = 0; i < shapes.size(); ++i) {
        const Shape &shape = shapes[i];
        const QRectF &bounds = shape.bounds;
        painter.setBrush(shapeColor(shape.type));
        painter.setPen(QPen(i == activeIndex ? Qt::red : Qt::black,
                            i == activeIndex ? 3 : 1));

        switch (shape.type) {
        case ShapeType::Rectangle:
            painter.drawRect(bounds);
            break;
        case ShapeType::Triangle: {
            QPolygonF triangle;
            triangle << QPointF(bounds.center().x(), bounds.top())
                     << bounds.bottomLeft() << bounds.bottomRight();
            painter.drawPolygon(triangle);
            break;
        }
        case ShapeType::Ellipse:
            painter.drawEllipse(bounds);
            break;
        }
    }
}

void WidgetCanvas::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton)
        return;

    activeIndex = -1;
    dragging = false;

    for (int i = shapes.size() - 1; i >= 0; --i) {
        if (shapes[i].bounds.contains(event->pos())) {
            const Shape selected = shapes.takeAt(i);
            shapes.append(selected);
            activeIndex = shapes.size() - 1;
            dragOffset = QPointF(event->pos()) - selected.bounds.topLeft();
            dragging = true;
            break;
        }
    }
    update();
}

void WidgetCanvas::mouseMoveEvent(QMouseEvent *event) {
    if (!dragging || activeIndex == -1 || !(event->buttons() & Qt::LeftButton))
        return;

    shapes[activeIndex].bounds.moveTopLeft(QPointF(event->pos()) - dragOffset);
    update();
}

void WidgetCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        dragging = false;
}
