#pragma once

#include "contact.h"

#include <QMainWindow>

class QCloseEvent;
class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QVector<Contact> contacts;
    QString currentFile;
    bool modified = false;

    QTableWidget *table;
    QLineEdit *nameSearch;
    QLineEdit *addressSearch;
    QLineEdit *emailSearch;
    QLineEdit *phoneSearch;
    QPushButton *editButton;
    QPushButton *removeButton;
    QLabel *countLabel;

    int selectedContactIndex() const;
    void refreshTable(int selectedIndex = -1);
    void applyFilters();
    void updateSelectionButtons();
    void setModified(bool value);

    void addContact();
    void editContact();
    void removeContact();
    void newBook();
    void openBook();
    bool saveBook();
    bool saveBookAs();
    bool writeBook(const QString &fileName);
    bool confirmUnsavedChanges();
};
