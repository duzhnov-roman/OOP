#include "shapescene.h"

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPolygonF>
#include <QTransform>

ShapeScene::ShapeScene(QObject *parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, 800, 500);
    setBackgroundBrush(Qt::white);
}

void ShapeScene::addShape(ShapeType type) {
    const QRectF bounds(0, 0, 140, 100);
    const QPen pen(Qt::black);
    const QBrush brush(shapeColor(type));
    const int offset = (items().size() % 6) * 35;
    QGraphicsItem *item = nullptr;

    switch (type) {
    case ShapeType::Rectangle:
        item = addRect(bounds, pen, brush);
        break;
    case ShapeType::Triangle: {
        QPolygonF triangle;
        triangle << QPointF(bounds.center().x(), bounds.top())
                 << bounds.bottomLeft() << bounds.bottomRight();
        item = addPolygon(triangle, pen, brush);
        break;
    }
    case ShapeType::Ellipse:
        item = addEllipse(bounds, pen, brush);
        break;
    }

    if (!item)
        return;

    item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    item->setAcceptedMouseButtons(Qt::LeftButton);
    item->setPos(60 + offset, 60 + offset);
    clearSelection();
    item->setSelected(true);
    item->setZValue(++topZ);
}

void ShapeScene::removeActiveShape() {
    const auto selected = selectedItems();
    if (selected.isEmpty())
        return;

    QGraphicsItem *item = selected.first();
    removeItem(item);
    delete item;
}

void ShapeScene::clearShapes() {
    clear();
    topZ = 0;
}

void ShapeScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        clearSelection();
        if (item) {
            item->setSelected(true);
            item->setZValue(++topZ);
        }

        event->setModifiers(Qt::NoModifier);
    }

    QGraphicsScene::mousePressEvent(event);
}
