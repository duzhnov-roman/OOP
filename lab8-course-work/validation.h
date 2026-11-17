#pragma once

#include "contact.h"

namespace Validation {
bool normalizeName(QString &value);
bool normalizeAddress(QString &value);
bool normalizeEmail(QString &value);
bool normalizePhone(const QString &input, QString &digits);
bool parseBirthDate(const QString &input, QDate &date);

// Проверяет весь контакт и убирает незначимые пробелы.
// При ошибке исходный контакт не изменяется.
bool normalizeContact(Contact &contact, QString &error);
}
