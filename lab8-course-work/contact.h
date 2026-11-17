#pragma once

#include <QDate>
#include <QString>
#include <QStringList>
#include <QVector>

struct PhoneNumber {
    QString type;
    QString digits; // Только цифры, включая код страны, без плюса и разделителей.
};

struct Contact {
    QString firstName;
    QString lastName;
    QString patronymic;
    QString address;
    QDate birthDate;
    QString email;
    QVector<PhoneNumber> phones;
};

inline QStringList phoneTypes() {
    return {QStringLiteral("Рабочий"), QStringLiteral("Домашний"),
            QStringLiteral("Служебный")};
}
