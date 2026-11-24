#include "validation.h"

#include <QRegularExpression>

namespace Validation {

bool normalizeName(QString &value) {
    value = value.simplified().normalized(QString::NormalizationForm_C);
    // Lu — заглавная буква Unicode; L — любая буква; Nd — десятичная цифра.
    static const QRegularExpression pattern(
        QStringLiteral(R"(\A\p{Lu}(?:[\p{L}\p{Nd} -]*[\p{L}\p{Nd}])?\z)"));
    return pattern.match(value).hasMatch();
}

bool normalizeAddress(QString &value) {
    value = value.trimmed();
    static const QRegularExpression pattern(QStringLiteral(
        R"(\A(?=[\s\S]*[\p{L}\p{Nd}])[^\p{Cc}\p{Cs}\p{Zl}\p{Zp}]+\z)"));
    return pattern.match(value).hasMatch();
}

bool normalizeEmail(QString &value) {
    static const QRegularExpression spaces(QStringLiteral(R"(\s+)"),
                                           QRegularExpression::UseUnicodePropertiesOption);
    value.remove(spaces);
    static const QRegularExpression pattern(
        QStringLiteral(R"(\A[A-Za-z0-9]+@[A-Za-z0-9]+(?:\.[A-Za-z0-9]+)*\z)"));
    if (!pattern.match(value).hasMatch())
        return false;

    const int at = value.indexOf('@');
    value = value.left(at + 1) + value.mid(at + 1).toLower();
    return true;
}

bool normalizePhone(const QString &input, QString &digits) {
    const QString text = input.simplified();
    static const QRegularExpression format(QStringLiteral(
        R"(\A(?:\+ *|00 *|8 *)(?:[0-9]+|\([0-9]+\))(?:[ -]?(?:[0-9]+|\([0-9]+\)))*\z)"));
    if (!format.match(text).hasMatch())
        return false;

    QString result = text;
    static const QRegularExpression separators(QStringLiteral(R"([^0-9])"));
    result.remove(separators);

    if (text.startsWith(QStringLiteral("00"))) {
        result.remove(0, 2);
    } else if (!text.startsWith('+')) {
        if (result.size() != 11)
            return false;
        result[0] = QChar('7');
    }

    static const QRegularExpression number(QStringLiteral(R"(\A[1-9][0-9]{6,14}\z)"));
    if (!number.match(result).hasMatch())
        return false;

    digits = result;
    return true;
}

bool parseBirthDate(const QString &input, QDate &date) {
    static const QRegularExpression pattern(QStringLiteral(
        R"(\A(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[0-2])\.([0-9]{4})\z)"));
    const auto match = pattern.match(input.trimmed());
    if (!match.hasMatch())
        return false;

    const QDate result(match.captured(3).toInt(), match.captured(2).toInt(),
                       match.captured(1).toInt());
    if (!result.isValid() || result >= QDate::currentDate())
        return false;

    date = result;
    return true;
}

bool normalizeContact(Contact &contact, QString &error) {
    Contact checked = contact;
    error.clear();

    if (!normalizeName(checked.lastName))
        error = QStringLiteral("Фамилия должна начинаться с заглавной буквы. Разрешены буквы, "
                               "цифры, пробелы и дефис; дефис в конце запрещён.");
    else if (!normalizeName(checked.firstName))
        error = QStringLiteral("Некорректное имя: нужна заглавная первая буква, "
                               "допустимы буквы, цифры, пробелы и дефис внутри.");
    else if (!normalizeName(checked.patronymic))
        error = QStringLiteral("Некорректное отчество: нужна заглавная первая буква, "
                               "допустимы буквы, цифры, пробелы и дефис внутри.");
    else if (!normalizeAddress(checked.address))
        error = QStringLiteral("Укажите адрес с буквами или цифрами, без переносов строк "
                               "и управляющих символов.");
    else if (!normalizeEmail(checked.email))
        error = QStringLiteral("Некорректный e-mail. Пример: Ivan123@example.com. "
                               "Имя пользователя — латинские буквы и цифры; "
                               "в домене дополнительно разрешены разделяющие точки.");

    if (!error.isEmpty())
        return false;

    QDate checkedDate;
    if (!parseBirthDate(checked.birthDate.toString(QStringLiteral("dd.MM.yyyy")), checkedDate)) {
        error = QStringLiteral("Дата рождения должна существовать и быть раньше сегодняшней даты.");
        return false;
    }
    if (checked.phones.isEmpty()) {
        error = QStringLiteral("Добавьте хотя бы один телефон. Количество номеров каждого типа не ограничено.");
        return false;
    }

    static const QRegularExpression storedNumber(QStringLiteral(R"(\A[1-9][0-9]{6,14}\z)"));
    for (int i = 0; i < checked.phones.size(); ++i) {
        const PhoneNumber &phone = checked.phones[i];
        if (!phoneTypes().contains(phone.type) || !storedNumber.match(phone.digits).hasMatch()) {
            error = QStringLiteral("Телефон №%1: нужен допустимый тип и от 7 до 15 цифр "
                                   "с ненулевым кодом страны.").arg(i + 1);
            return false;
        }
    }

    contact = checked;
    return true;
}

} 