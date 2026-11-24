#pragma once

#include "contact.h"

namespace Validation {
bool normalizeName(QString &value);
bool normalizeSurname(QString &value);
bool matchesPhoneQuery(const QVector<PhoneNumber> &phones, const QString &input);
bool normalizeAddress(QString &value);
bool normalizeEmail(QString &value);
bool normalizePhone(const QString &input, QString &digits);
bool parseBirthDate(const QString &input, QDate &date);

bool normalizeContact(Contact &contact, QString &error);
}
