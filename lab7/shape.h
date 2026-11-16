#pragma once

#include <QColor>
#include <QRectF>

enum class ShapeType {
    Rectangle,
    Triangle,
    Ellipse
};

struct Shape {
    ShapeType type;
    QRectF bounds;
};

inline QColor shapeColor(ShapeType type) {
    switch (type) {
    case ShapeType::Rectangle:
        return QColor(100, 170, 240); // голубой
    case ShapeType::Triangle:
        return QColor(250, 190, 90); // оранжевый
    case ShapeType::Ellipse:
        return QColor(120, 205, 150); // зеленый
    }
    return Qt::gray;
}
