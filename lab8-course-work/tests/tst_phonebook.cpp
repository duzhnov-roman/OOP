#include "contactstore.h"
#include "validation.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QtTest>

namespace {

Contact exampleContact() {
    Contact contact;
    contact.firstName = QStringLiteral("Иван");
    contact.lastName = QStringLiteral("Петров");
    contact.patronymic = QStringLiteral("Сергеевич");
    contact.address = QStringLiteral("Санкт-Петербург, ул. Мира, д. 10, кв. 2");
    contact.birthDate = QDate(2000, 2, 29);
    contact.email = QStringLiteral("Ivan123@example.com");
    contact.phones = {{phoneTypes()[0], QStringLiteral("78121234567")},
                      {phoneTypes()[0], QStringLiteral("78001231212")},
                      {phoneTypes()[1], QStringLiteral("380321231111")}};
    return contact;
}

} // namespace

class PhoneBookTests : public QObject {
    Q_OBJECT

private slots:
    void names_data() {
        QTest::addColumn<QString>("input");
        QTest::addColumn<bool>("valid");
        QTest::newRow("cyrillic") << QStringLiteral("Ёлкин") << true;
        QTest::newRow("trim") << QStringLiteral("  Анна-Мария  ") << true;
        QTest::newRow("latin digits") << QStringLiteral("Jean Paul2") << true;
        QTest::newRow("greek") << QStringLiteral("Αλέξανδρος") << true;
        QTest::newRow("single letter") << QStringLiteral("И") << true;
        QTest::newRow("empty") << QString() << false;
        QTest::newRow("lowercase") << QStringLiteral("иван") << false;
        QTest::newRow("digit first") << QStringLiteral("2Иван") << false;
        QTest::newRow("leading hyphen") << QStringLiteral("-Иван") << false;
        QTest::newRow("trailing hyphen") << QStringLiteral("Иван- ") << false;
        QTest::newRow("punctuation") << QStringLiteral("Иван!") << false;
    }

    void names() {
        QFETCH(QString, input);
        QFETCH(bool, valid);
        QCOMPARE(Validation::normalizeName(input), valid);
        if (valid)
            QCOMPARE(input, input.trimmed());
    }

    void phones_data() {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("expected");
        QTest::newRow("international") << QStringLiteral("+7 812 1234567")
                                        << QStringLiteral("78121234567");
        QTest::newRow("russian trunk") << QStringLiteral("8(800)123-1212")
                                        << QStringLiteral("78001231212");
        QTest::newRow("spaces parentheses") << QStringLiteral("+ 38 (032) 123 11 11")
                                             << QStringLiteral("380321231111");
        QTest::newRow("00 prefix") << QStringLiteral("00 44 20 1234 5678")
                                    << QStringLiteral("442012345678");
        QTest::newRow("trim") << QStringLiteral("  +7 (812) 123-45-67  ")
                               << QStringLiteral("78121234567");
        QTest::newRow("empty") << QString() << QString();
        QTest::newRow("short") << QStringLiteral("+123") << QString();
        QTest::newRow("long") << QStringLiteral("+1234567890123456") << QString();
        QTest::newRow("no prefix") << QStringLiteral("78121234567") << QString();
        QTest::newRow("letters") << QStringLiteral("+7 812 abc4567") << QString();
        QTest::newRow("zero country") << QStringLiteral("+0123456789") << QString();
        QTest::newRow("unclosed parentheses") << QStringLiteral("+7 (812 1234567") << QString();
        QTest::newRow("extra close") << QStringLiteral("+7 812)1234567") << QString();
        QTest::newRow("two pluses") << QStringLiteral("++78121234567") << QString();
        QTest::newRow("double separator") << QStringLiteral("+7--8121234567") << QString();
    }

    void phones() {
        QFETCH(QString, input);
        QFETCH(QString, expected);
        QString digits = QStringLiteral("unchanged");
        QCOMPARE(Validation::normalizePhone(input, digits), !expected.isEmpty());
        QCOMPARE(digits, expected.isEmpty() ? QStringLiteral("unchanged") : expected);
    }

    void emails_data() {
        QTest::addColumn<QString>("input");
        QTest::addColumn<QString>("expected");
        QTest::newRow("normal") << QStringLiteral("Ivan123@example.com")
                                 << QStringLiteral("Ivan123@example.com");
        QTest::newRow("whitespace") << QStringLiteral("  Ivan 123 @ EXAMPLE . com  ")
                                     << QStringLiteral("Ivan123@example.com");
        QTest::newRow("subdomain") << QStringLiteral("user1@mail2.example.com")
                                    << QStringLiteral("user1@mail2.example.com");
        QTest::newRow("single domain label") << QStringLiteral("user@domain")
                                              << QStringLiteral("user@domain");
        QTest::newRow("no at") << QStringLiteral("userexample.com") << QString();
        QTest::newRow("two ats") << QStringLiteral("user@@example.com") << QString();
        QTest::newRow("cyrillic") << QStringLiteral("иван@example.com") << QString();
        QTest::newRow("underscore") << QStringLiteral("user_name@example.com") << QString();
        QTest::newRow("username dot") << QStringLiteral("user.name@example.com") << QString();
        QTest::newRow("empty label") << QStringLiteral("user@example..com") << QString();
    }

    void emails() {
        QFETCH(QString, input);
        QFETCH(QString, expected);
        QCOMPARE(Validation::normalizeEmail(input), !expected.isEmpty());
        if (!expected.isEmpty())
            QCOMPARE(input, expected);
    }

    void unicodeEmailWhitespace() {
        QString email = QStringLiteral(" Ivan123") + QChar(0x00A0)
                        + QStringLiteral("@") + QChar(0x00A0) + QStringLiteral("EXAMPLE.com ");
        QVERIFY(Validation::normalizeEmail(email));
        QCOMPARE(email, QStringLiteral("Ivan123@example.com"));
    }

    void dates_data() {
        QTest::addColumn<QString>("input");
        QTest::addColumn<bool>("valid");
        QTest::newRow("leap 2000") << QStringLiteral("29.02.2000") << true;
        QTest::newRow("leap 2024") << QStringLiteral("29.02.2024") << true;
        QTest::newRow("not leap 1900") << QStringLiteral("29.02.1900") << false;
        QTest::newRow("not leap 2023") << QStringLiteral("29.02.2023") << false;
        QTest::newRow("april 31") << QStringLiteral("31.04.2000") << false;
        QTest::newRow("month 13") << QStringLiteral("01.13.2000") << false;
        QTest::newRow("day zero") << QStringLiteral("00.01.2000") << false;
        QTest::newRow("year zero") << QStringLiteral("01.01.0000") << false;
        QTest::newRow("wrong format") << QStringLiteral("2000-01-01") << false;
        QTest::newRow("today") << QDate::currentDate().toString("dd.MM.yyyy") << false;
        QTest::newRow("tomorrow") << QDate::currentDate().addDays(1).toString("dd.MM.yyyy") << false;
        QTest::newRow("yesterday") << QDate::currentDate().addDays(-1).toString("dd.MM.yyyy") << true;
    }

    void dates() {
        QFETCH(QString, input);
        QFETCH(bool, valid);
        QDate date;
        QCOMPARE(Validation::parseBirthDate(input, date), valid);
    }

    void contactValidation() {
        Contact contact = exampleContact();
        contact.firstName = QStringLiteral("  Иван  ");
        contact.email = QStringLiteral(" Ivan123 @ EXAMPLE.com ");
        QString error;
        QVERIFY2(Validation::normalizeContact(contact, error), qPrintable(error));
        QCOMPARE(contact.firstName, QStringLiteral("Иван"));
        QCOMPARE(contact.email, QStringLiteral("Ivan123@example.com"));
        QCOMPARE(contact.phones.size(), 3);

        contact.address.clear();
        QVERIFY(!Validation::normalizeContact(contact, error));
        contact = exampleContact();
        contact.phones.clear();
        QVERIFY(!Validation::normalizeContact(contact, error));
        contact = exampleContact();
        contact.phones[0].type = QStringLiteral("Неизвестный");
        QVERIFY(!Validation::normalizeContact(contact, error));
        contact = exampleContact();
        contact.phones[0].digits = QStringLiteral("+78121234567");
        QVERIFY(!Validation::normalizeContact(contact, error));
    }

    void failedValidationKeepsOriginal() {
        Contact contact = exampleContact();
        contact.firstName = QStringLiteral("  Иван  ");
        contact.email = QStringLiteral("bad email");
        QString error;
        QVERIFY(!Validation::normalizeContact(contact, error));
        QCOMPARE(contact.firstName, QStringLiteral("  Иван  "));
        QVERIFY(!error.isEmpty());
    }

    void addresses() {
        QString address = QStringLiteral("  Москва, ул. Мира, д. 1/2  ");
        QVERIFY(Validation::normalizeAddress(address));
        QCOMPARE(address, QStringLiteral("Москва, ул. Мира, д. 1/2"));
        address = QStringLiteral("---");
        QVERIFY(!Validation::normalizeAddress(address));
        address = QStringLiteral("Москва\nМира");
        QVERIFY(!Validation::normalizeAddress(address));
    }

    void fileRoundTrip() {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        const QString path = directory.filePath(QStringLiteral("справочник.json"));
        QString error;
        QVERIFY2(ContactStore::save(path, {exampleContact()}, error), qPrintable(error));
        QVector<Contact> loaded;
        QVERIFY2(ContactStore::load(path, loaded, error), qPrintable(error));
        QCOMPARE(loaded.size(), 1);
        const Contact expected = exampleContact();
        QCOMPARE(loaded[0].lastName, expected.lastName);
        QCOMPARE(loaded[0].firstName, expected.firstName);
        QCOMPARE(loaded[0].patronymic, expected.patronymic);
        QCOMPARE(loaded[0].address, expected.address);
        QCOMPARE(loaded[0].birthDate, expected.birthDate);
        QCOMPARE(loaded[0].email, expected.email);
        QCOMPARE(loaded[0].phones.size(), expected.phones.size());
        for (int i = 0; i < loaded[0].phones.size(); ++i) {
            QCOMPARE(loaded[0].phones[i].type, expected.phones[i].type);
            QCOMPARE(loaded[0].phones[i].digits, expected.phones[i].digits);
        }

        QVERIFY(ContactStore::save(path, {}, error));
        QVERIFY(ContactStore::load(path, loaded, error));
        QVERIFY(loaded.isEmpty());
    }

    void invalidFileKeepsCurrentContacts() {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        const QString path = directory.filePath("broken.json");
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        QVERIFY(file.write("not JSON") > 0);
        file.close();
        QVector<Contact> current = {exampleContact()};
        QString error;
        QVERIFY(!ContactStore::load(path, current, error));
        QCOMPARE(current.size(), 1);
        QCOMPARE(current[0].firstName, QStringLiteral("Иван"));

        QVERIFY(ContactStore::save(path, current, error));
        QVERIFY(file.open(QIODevice::ReadOnly));
        QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
        QJsonArray entries = root.value("contacts").toArray();
        QJsonObject bad = entries.first().toObject();
        bad["birthDate"] = QStringLiteral("31.02.2000");
        entries.append(bad); // Первая запись верна, вторая нет: нельзя загрузить только первую.
        root["contacts"] = entries;
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        QVERIFY(file.write(QJsonDocument(root).toJson()) > 0);
        file.close();
        QVERIFY(!ContactStore::load(path, current, error));
        QCOMPARE(current.size(), 1);
    }

    void invalidSaveKeepsExistingFile() {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        const QString path = directory.filePath("contacts.json");
        QString error;
        QVERIFY(ContactStore::save(path, {exampleContact()}, error));
        Contact invalid = exampleContact();
        invalid.firstName = QStringLiteral("иван");
        QVERIFY(!ContactStore::save(path, {invalid}, error));
        QVector<Contact> loaded;
        QVERIFY(ContactStore::load(path, loaded, error));
        QCOMPARE(loaded.size(), 1);
        QCOMPARE(loaded[0].firstName, QStringLiteral("Иван"));
        QVERIFY(!ContactStore::save(directory.filePath("missing/contacts.json"), loaded, error));
        QVERIFY(!ContactStore::load(directory.filePath("missing.json"), loaded, error));
        QCOMPARE(loaded.size(), 1);
    }
};

QTEST_GUILESS_MAIN(PhoneBookTests)
#include "tst_phonebook.moc"
