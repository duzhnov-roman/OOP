#pragma once

#include "contact.h"

#include <QDialog>

class QDateEdit;
class QLineEdit;
class QTableWidget;

class ContactDialog : public QDialog {
public:
    explicit ContactDialog(const Contact &contact = Contact{}, QWidget *parent = nullptr);
    Contact contact() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *patronymicEdit;
    QLineEdit *addressEdit;
    QDateEdit *birthDateEdit;
    QLineEdit *emailEdit;
    QTableWidget *phoneTable;
    Contact result;

    void addPhoneRow(const PhoneNumber &phone = PhoneNumber{});
    void removePhoneRow();
    void saveContact();
};
