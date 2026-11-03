#include "mywidget.h"
#include <QApplication>

MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
    setFixedSize(300, 200);
    setWindowTitle("Lab 1: Hello World");

    helloLabel = new QLabel("Hello World", this);
    helloLabel->setGeometry(100, 50, 100, 30);
    helloLabel->setAlignment(Qt::AlignCenter);

    exitButton = new QPushButton("Выход", this);
    exitButton->setGeometry(100, 100, 100, 30);

    connect(exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);
}