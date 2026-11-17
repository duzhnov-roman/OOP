#include "contactstore.h"
#include "validation.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSaveFile>

namespace ContactStore {

bool load(const QString &fileName, QVector<Contact> &contacts, QString &error) {
    error.clear();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        error = file.errorString();
        return false;
    }
    const QByteArray bytes = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        error = file.errorString();
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        error = QStringLiteral("Файл должен содержать JSON-объект. %1").arg(parseError.errorString());
        return false;
    }
    const QJsonObject root = document.object();
    if (root.value("version").toDouble(-1) != 1 || !root.value("contacts").isArray()) {
        error = QStringLiteral("Неизвестный формат файла: нужны version = 1 и массив contacts.");
        return false;
    }

    QVector<Contact> loaded;
    const QJsonArray entries = root.value("contacts").toArray();
    for (int i = 0; i < entries.size(); ++i) {
        if (!entries[i].isObject()) {
            error = QStringLiteral("Запись №%1 должна быть объектом.").arg(i + 1);
            return false;
        }
        const QJsonObject object = entries[i].toObject();
        const QStringList fields = {"firstName", "lastName", "patronymic", "address", "birthDate", "email"};
        for (const QString &field : fields) {
            if (!object.value(field).isString()) {
                error = QStringLiteral("Запись №%1: поле %2 должно быть строкой.").arg(i + 1).arg(field);
                return false;
            }
        }

        Contact contact;
        contact.firstName = object.value("firstName").toString();
        contact.lastName = object.value("lastName").toString();
        contact.patronymic = object.value("patronymic").toString();
        contact.address = object.value("address").toString();
        contact.email = object.value("email").toString();
        if (!Validation::parseBirthDate(object.value("birthDate").toString(), contact.birthDate)) {
            error = QStringLiteral("Запись №%1: некорректная дата рождения.").arg(i + 1);
            return false;
        }
        if (!object.value("phones").isArray()) {
            error = QStringLiteral("Запись №%1: phones должен быть массивом.").arg(i + 1);
            return false;
        }
        for (const QJsonValue &value : object.value("phones").toArray()) {
            if (!value.isObject() || !value.toObject().value("type").isString()
                || !value.toObject().value("number").isString()) {
                error = QStringLiteral("Запись №%1: каждый телефон должен содержать строки type и number.")
                            .arg(i + 1);
                return false;
            }
            const QJsonObject phone = value.toObject();
            contact.phones.append(PhoneNumber{phone.value("type").toString(),
                                             phone.value("number").toString()});
        }
        QString details;
        if (!Validation::normalizeContact(contact, details)) {
            error = QStringLiteral("Запись №%1: %2").arg(i + 1).arg(details);
            return false;
        }
        loaded.append(contact);
    }
    contacts = loaded;
    return true;
}

bool save(const QString &fileName, const QVector<Contact> &contacts, QString &error) {
    error.clear();
    QJsonArray entries;
    for (int i = 0; i < contacts.size(); ++i) {
        Contact contact = contacts[i];
        if (!Validation::normalizeContact(contact, error)) {
            error = QStringLiteral("Запись №%1: %2").arg(i + 1).arg(error);
            return false;
        }
        QJsonArray phones;
        for (const PhoneNumber &phone : contact.phones) {
            phones.append(QJsonObject{{"type", phone.type}, {"number", phone.digits}});
        }
        entries.append(QJsonObject{
            {"firstName", contact.firstName}, {"lastName", contact.lastName},
            {"patronymic", contact.patronymic}, {"address", contact.address},
            {"birthDate", contact.birthDate.toString(QStringLiteral("dd.MM.yyyy"))},
            {"email", contact.email}, {"phones", phones}
        });
    }

    const QJsonDocument document(QJsonObject{{"version", 1}, {"contacts", entries}});
    const QByteArray bytes = document.toJson(QJsonDocument::Indented);
    // QSaveFile пишет во временный файл и заменяет исходный только при успехе.
    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        error = file.errorString();
        return false;
    }
    if (file.write(bytes) != bytes.size()) {
        error = file.errorString();
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        error = file.errorString();
        return false;
    }
    return true;
}

}
