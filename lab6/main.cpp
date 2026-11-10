
#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMetaMethod>
#include <QPointer>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

class WidgetCollection : public QWidget {
public:
    WidgetCollection() {
        auto *mainLayout = new QVBoxLayout(this);
        auto *buttons = new QHBoxLayout;

        auto *addButton = new QPushButton("Добавить", this);
        auto *connectButton = new QPushButton("Соединить", this);
        auto *removeButton = new QPushButton("Удалить последний", this);

        buttons->addWidget(addButton);
        buttons->addWidget(connectButton);
        buttons->addWidget(removeButton);
        mainLayout->addLayout(buttons);

        widgetsLayout = new QVBoxLayout;
        mainLayout->addLayout(widgetsLayout);
        mainLayout->addStretch();

        connect(addButton, &QPushButton::clicked,
                this, &WidgetCollection::addWidget);
        connect(connectButton, &QPushButton::clicked,
                this, &WidgetCollection::connectWidgets);
        connect(removeButton, &QPushButton::clicked,
                this, &WidgetCollection::removeLastWidget);

        setWindowTitle("Коллекция виджетов");
        resize(480, 320);
    }

private:
    QList<QWidget *> widgets;
    QVBoxLayout *widgetsLayout = nullptr;

    void addWidget() {
        QWidget *widget = nullptr;

        switch (widgets.size() % 4) {
        case 0:
            widget = new QLabel("0", this);
            break;

        case 1: {
            auto *slider = new QSlider(Qt::Horizontal, this);
            slider->setRange(0, 100);
            widget = slider;
            break;
        }

        case 2: {
            auto *scrollBar = new QScrollBar(Qt::Horizontal, this);
            scrollBar->setRange(0, 100);
            widget = scrollBar;
            break;
        }

        default: {
            auto *spinBox = new QSpinBox(this);
            spinBox->setRange(0, 100);
            widget = spinBox;
        }
        }

        widgets.append(widget);
        widgetsLayout->addWidget(widget);

        qDebug() << "Добавлен:"
                 << widget->metaObject()->className();

    }

    void connectWidgets() {
        int added = 0;
        int duplicates = 0;

        for (QWidget *source : widgets) {
            const QMetaObject *sourceMeta = source->metaObject();

            if (sourceMeta->inherits(&QLabel::staticMetaObject))
                continue;

            const int signalIndex =
                sourceMeta->indexOfSignal("valueChanged(int)");

            Q_ASSERT(signalIndex >= 0);
            if (signalIndex < 0)
                continue;

            const QMetaMethod signal = sourceMeta->method(signalIndex);

            for (QWidget *target : widgets) {
                if (source == target)
                    continue;

                const QMetaObject *targetMeta = target->metaObject();

                const char *slotName =
                    targetMeta->inherits(&QLabel::staticMetaObject)
                    ? "setNum(int)" : "setValue(int)";

                const int slotIndex = targetMeta->indexOfSlot(slotName);

                Q_ASSERT(slotIndex >= 0);
                if (slotIndex < 0)
                    continue;

                const QMetaMethod slot = targetMeta->method(slotIndex);

                if (QObject::connect(source, signal, target, slot,
                                     Qt::UniqueConnection))
                    ++added;
                else
                    ++duplicates;
            }
        }

        qDebug() << "Новых соединений:" << added
                 << "| Повторных отклонено:" << duplicates;

#ifndef QT_NO_DEBUG
        for (QWidget *widget : widgets)
            widget->dumpObjectInfo();
#endif
    }

    void removeLastWidget() {
        if (widgets.isEmpty())
            return;

        QPointer<QWidget> removed(widgets.takeLast());

        delete removed.data();

        Q_ASSERT(removed.isNull());

        qDebug() << "Виджет удалён. Указатель обнулён:"
                 << removed.isNull();

#ifndef QT_NO_DEBUG
        for (QWidget *widget : widgets)
            widget->dumpObjectInfo();
#endif
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WidgetCollection window;
    window.show();

    return app.exec();
}
