#pragma once

#include "shape.h"

#include <QGraphicsScene>

class ShapeScene : public QGraphicsScene {
public:
    explicit ShapeScene(QObject *parent = nullptr);

    void addShape(ShapeType type);
    void removeActiveShape();
    void clearShapes();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    qreal topZ = 0;
};
