#pragma once

#include "contact.h"

namespace ContactStore {
bool load(const QString &fileName, QVector<Contact> &contacts, QString &error);
bool save(const QString &fileName, const QVector<Contact> &contacts, QString &error);
}
