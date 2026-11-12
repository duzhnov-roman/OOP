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
        auto *deleteButton = new QPushButton("Удалить по номеру", this);

        indexSpinBox = new QSpinBox(this);
        indexSpinBox->setRange(0, 0);

        buttons->addWidget(addButton);
        buttons->addWidget(connectButton);
        buttons->addWidget(removeButton);
        buttons->addWidget(indexSpinBox);
        buttons->addWidget(deleteButton);
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
        connect(deleteButton, &QPushButton::clicked,
                this, &WidgetCollection::removeWidgetByNumber);

        setWindowTitle("Коллекция виджетов");
        resize(750, 320);
    }

private:
    QList<QWidget *> widgets;
    QVBoxLayout *widgetsLayout = nullptr;
    QSpinBox *indexSpinBox = nullptr;

    void updateSpinBoxRange() {
        int count = widgets.size();
        indexSpinBox->setRange(count == 0 ? 0 : 1, count);
    }

    void updateWidgetNames() {
        for (int i = 0; i < widgets.size(); ++i) {
            QWidget *widget = widgets[i];
            widget->setObjectName(QString("%1").arg(i + 1));
        }
    }

    int currentValue() const {
        for (QWidget *widget : widgets) {
            if (!widget->metaObject()->inherits(&QLabel::staticMetaObject))
                return widget->property("value").toInt();
        }
        return 0;
    }

    void debugConnections() const {
#ifndef QT_NO_DEBUG
        for (QWidget *widget : widgets)
            widget->dumpObjectInfo();
#endif
    }

    void addWidget() {
        const int value = currentValue();
        QWidget *widget = nullptr;

        switch (widgets.size() % 4) {
        case 0:
            widget = new QLabel(QString::number(value), this);
            break;
        case 1: {
            auto *slider = new QSlider(Qt::Horizontal, this);
            slider->setRange(0, 100);
            slider->setValue(value);
            widget = slider;
            break;
        }
        case 2: {
            auto *scrollBar = new QScrollBar(Qt::Horizontal, this);
            scrollBar->setRange(0, 100);
            scrollBar->setValue(value);
            widget = scrollBar;
            break;
        }
        default: {
            auto *spinBox = new QSpinBox(this);
            spinBox->setRange(0, 100);
            spinBox->setValue(value);
            widget = spinBox;
            break;
        }
        }

        widgets.append(widget);
        widgetsLayout->addWidget(widget);
        updateWidgetNames();
        updateSpinBoxRange();

        qDebug() << "Добавлен:" << widget->objectName()
                 << "| Начальное значение:" << value;
    }

    void connectWidgets() {
        const int value = currentValue();
        int added = 0;
        int duplicates = 0;

        for (QWidget *source : widgets) {
            const QMetaObject *sourceMeta = source->metaObject();
            if (sourceMeta->inherits(&QLabel::staticMetaObject))
                continue;

            int signalIndex = sourceMeta->indexOfSignal("valueChanged(int)");
            Q_ASSERT(signalIndex >= 0);
            if (signalIndex < 0)
                continue;

            QMetaMethod signal = sourceMeta->method(signalIndex);

            for (QWidget *target : widgets) {
                if (source == target)
                    continue;

                const QMetaObject *targetMeta = target->metaObject();
                const char *slotName =
                    targetMeta->inherits(&QLabel::staticMetaObject)
                    ? "setNum(int)" : "setValue(int)";

                int slotIndex = targetMeta->indexOfSlot(slotName);
                Q_ASSERT(slotIndex >= 0);
                if (slotIndex < 0)
                    continue;

                QMetaMethod slot = targetMeta->method(slotIndex);

                if (QObject::connect(source, signal, target, slot,
                                     Qt::UniqueConnection))
                    ++added;
                else
                    ++duplicates;
            }
        }

        for (QWidget *widget : widgets) {
            if (widget->metaObject()->inherits(&QLabel::staticMetaObject)) {
                QMetaObject::invokeMethod(widget, "setNum", Qt::DirectConnection, Q_ARG(int, value));
            } else {
                widget->setProperty("value", value);
            }
        }

        qDebug() << "Новых соединений:" << added
                 << "| Повторных отклонено:" << duplicates
                 << "| Общее значение:" << value;
        debugConnections();
    }

    void removeLastWidget() {
        if (widgets.isEmpty())
            return;

        QPointer<QWidget> removed(widgets.takeLast());
        QString name = removed->objectName();
        delete removed.data();
        Q_ASSERT(removed.isNull());

        updateWidgetNames();
        updateSpinBoxRange();

        qDebug() << "Удалён:" << name
                 << "| Указатель обнулён:" << removed.isNull();
        debugConnections();
    }

    void removeWidgetByNumber() {
        if (widgets.isEmpty())
            return;

        int index = indexSpinBox->value() - 1;
        QPointer<QWidget> removed(widgets.takeAt(index));
        QString name = removed->objectName();
        delete removed.data();
        Q_ASSERT(removed.isNull());

        updateWidgetNames();
        updateSpinBoxRange();

        qDebug() << "Удалён номер:" << index + 1
                 << "| Имя:" << name
                 << "| Указатель обнулён:" << removed.isNull();
        debugConnections();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WidgetCollection window;
    window.show();
    return app.exec();
}
