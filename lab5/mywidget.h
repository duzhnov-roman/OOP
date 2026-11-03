#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class MyWidget : public QWidget {
    Q_OBJECT

public:
    MyWidget(QWidget *parent = nullptr);

private:
    QLabel *helloLabel;
    QPushButton *exitButton;
};