#include "contactdialog.h"
#include "validation.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

ContactDialog::ContactDialog(const Contact &contact, QWidget *parent) : QDialog(parent) {
    setWindowTitle(QStringLiteral("Контакт"));
    resize(620, 640);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;
    lastNameEdit = new QLineEdit(contact.lastName);
    firstNameEdit = new QLineEdit(contact.firstName);
    patronymicEdit = new QLineEdit(contact.patronymic);
    addressEdit = new QLineEdit(contact.address);
    emailEdit = new QLineEdit(contact.email);
    birthDateEdit = new QDateEdit;
    birthDateEdit->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setMinimumDate(QDate(1, 1, 1));
    birthDateEdit->setMaximumDate(QDate::currentDate().addDays(-1));
    birthDateEdit->setDate(contact.birthDate.isValid() ? contact.birthDate
                                                    : QDate::currentDate().addYears(-20));
    form->addRow(QStringLiteral("Фамилия:"), lastNameEdit);
    form->addRow(QStringLiteral("Имя:"), firstNameEdit);
    form->addRow(QStringLiteral("Отчество:"), patronymicEdit);
    form->addRow(QStringLiteral("Адрес:"), addressEdit);
    form->addRow(QStringLiteral("Дата рождения:"), birthDateEdit);
    form->addRow(QStringLiteral("E-mail:"), emailEdit);
    layout->addLayout(form);

    const QString nameHint = QStringLiteral("Начните с заглавной буквы. Разрешены буквы разных "
                                           "алфавитов, цифры, пробелы и дефис внутри строки.");
    firstNameEdit->setToolTip(nameHint);
    lastNameEdit->setToolTip(nameHint);
    patronymicEdit->setToolTip(nameHint);
    emailEdit->setPlaceholderText(QStringLiteral("Ivan123@example.com"));
    emailEdit->setToolTip(QStringLiteral("Имя пользователя: латинские буквы и цифры. "
                                        "Пробелы удаляются автоматически."));

    auto *hint = new QLabel(QStringLiteral("Все поля обязательны. Добавьте хотя бы один номер. "
                                           "Номеров каждого типа может быть сколько угодно."));
    hint->setWordWrap(true);
    layout->addWidget(hint);

    phoneTable = new QTableWidget(0, 2);
    phoneTable->setHorizontalHeaderLabels({QStringLiteral("Тип"), QStringLiteral("Номер")});
    phoneTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    phoneTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    phoneTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    phoneTable->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(phoneTable);
    for (const PhoneNumber &phone : contact.phones)
        addPhoneRow(phone);
    if (contact.phones.isEmpty())
        addPhoneRow();

    auto *phoneButtons = new QHBoxLayout;
    auto *addButton = new QPushButton(QStringLiteral("Добавить номер"));
    auto *removeButton = new QPushButton(QStringLiteral("Удалить номер"));
    phoneButtons->addWidget(addButton);
    phoneButtons->addWidget(removeButton);
    phoneButtons->addStretch();
    layout->addLayout(phoneButtons);
    connect(addButton, &QPushButton::clicked, this, [this]() { addPhoneRow(); });
    connect(removeButton, &QPushButton::clicked, this, &ContactDialog::removePhoneRow);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Save)->setText(QStringLiteral("Сохранить"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Отмена"));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &ContactDialog::saveContact);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Contact ContactDialog::contact() const {
    return result;
}

void ContactDialog::addPhoneRow(const PhoneNumber &phone) {
    const int row = phoneTable->rowCount();
    phoneTable->insertRow(row);
    auto *typeBox = new QComboBox;
    typeBox->addItems(phoneTypes());
    if (!phone.type.isEmpty())
        typeBox->setCurrentText(phone.type);
    auto *numberEdit = new QLineEdit;
    numberEdit->setPlaceholderText(QStringLiteral("+7 812 1234567"));
    numberEdit->setToolTip(QStringLiteral("Например: +7 812 1234567; 8(800)123-1212; "
                                         "+ 38 (032) 123 11 11. Допустим также префикс 00."));
    if (!phone.digits.isEmpty())
        numberEdit->setText(QStringLiteral("+") + phone.digits);
    phoneTable->setCellWidget(row, 0, typeBox);
    phoneTable->setCellWidget(row, 1, numberEdit);
    typeBox->installEventFilter(this);
    numberEdit->installEventFilter(this);
    phoneTable->setCurrentCell(row, 1);
}

bool ContactDialog::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::FocusIn) {
        for (int row = 0; row < phoneTable->rowCount(); ++row) {
            for (int column = 0; column < phoneTable->columnCount(); ++column) {
                if (phoneTable->cellWidget(row, column) == watched)
                    phoneTable->setCurrentCell(row, column);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

void ContactDialog::removePhoneRow() {
    if (phoneTable->currentRow() >= 0)
        phoneTable->removeRow(phoneTable->currentRow());
}

void ContactDialog::saveContact() {
    Contact candidate;
    candidate.lastName = lastNameEdit->text();
    candidate.firstName = firstNameEdit->text();
    candidate.patronymic = patronymicEdit->text();
    candidate.address = addressEdit->text();
    candidate.birthDate = birthDateEdit->date();
    candidate.email = emailEdit->text();

    for (int row = 0; row < phoneTable->rowCount(); ++row) {
        auto *typeBox = static_cast<QComboBox *>(phoneTable->cellWidget(row, 0));
        auto *numberEdit = static_cast<QLineEdit *>(phoneTable->cellWidget(row, 1));
        QString digits;
        if (!Validation::normalizePhone(numberEdit->text(), digits)) {
            QMessageBox::warning(this, QStringLiteral("Ошибка ввода"),
                QStringLiteral("Телефон №%1 записан неверно. Используйте + или 00 перед кодом страны, "
                               "7–15 цифр. Для российского номера допустима запись 8(800)123-1212.")
                    .arg(row + 1));
            phoneTable->setCurrentCell(row, 1);
            numberEdit->setFocus();
            return;
        }
        candidate.phones.append(PhoneNumber{typeBox->currentText(), digits});
    }

    QString error;
    if (!Validation::normalizeContact(candidate, error)) {
        QMessageBox::warning(this, QStringLiteral("Ошибка ввода"), error);
        return;
    }
    result = candidate;
    accept();
}
