#include "shapescene.h"
#include "widgetcanvas.h"

#include <QApplication>
#include <QComboBox>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.setWindowTitle("Лабораторная работа 7");
    window.resize(900, 650);

    auto *layout = new QVBoxLayout(&window);
    auto *buttons = new QHBoxLayout;
    auto *typeBox = new QComboBox;
    typeBox->addItem("Прямоугольник", static_cast<int>(ShapeType::Rectangle));
    typeBox->addItem("Треугольник", static_cast<int>(ShapeType::Triangle));
    typeBox->addItem("Эллипс", static_cast<int>(ShapeType::Ellipse));
    auto *addButton = new QPushButton("Добавить");
    auto *removeButton = new QPushButton("Удалить активную");
    auto *clearButton = new QPushButton("Очистить");
    buttons->addWidget(typeBox);
    buttons->addWidget(addButton);
    buttons->addWidget(removeButton);
    buttons->addWidget(clearButton);
    buttons->addStretch();
    layout->addLayout(buttons);

    auto *hint = new QLabel("ЛКМ - выбрать и поднять фигуру. Удерживайте ЛКМ для перемещения.");
    hint->setWordWrap(true);
    layout->addWidget(hint);

    auto *tabs = new QTabWidget;
    auto *canvas = new WidgetCanvas;
    auto *scene = new ShapeScene(&window);
    auto *view = new QGraphicsView(scene);

    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setRenderHint(QPainter::Antialiasing);
    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view->setMinimumSize(640, 420);
    tabs->addTab(canvas, "7.1 - Рисование на QWidget");
    tabs->addTab(view, "7.2 - QGraphicsScene");
    layout->addWidget(tabs);

    QObject::connect(addButton, &QPushButton::clicked, &window, [=]() {
        const auto type = static_cast<ShapeType>(typeBox->currentData().toInt());
        if (tabs->currentIndex() == 0)
            canvas->addShape(type);
        else
            scene->addShape(type);
    });
    QObject::connect(removeButton, &QPushButton::clicked, &window, [=]() {
        if (tabs->currentIndex() == 0)
            canvas->removeActiveShape();
        else
            scene->removeActiveShape();
    });
    QObject::connect(clearButton, &QPushButton::clicked, &window, [=]() {
        if (tabs->currentIndex() == 0)
            canvas->clearShapes();
        else
            scene->clearShapes();
    });

    for (ShapeType type : {ShapeType::Rectangle, ShapeType::Triangle, ShapeType::Ellipse}) {
        canvas->addShape(type);
        scene->addShape(type);
    }

    window.show();
    return app.exec();
}
