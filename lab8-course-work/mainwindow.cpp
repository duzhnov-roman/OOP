#include "mainwindow.h"
#include "contactdialog.h"
#include "contactstore.h"
#include "validation.h"

#include <QCalendarWidget>
#include <QHelpEvent>
#include <QScreen>
#include <QGuiApplication>

#include <QAbstractItemModel>
#include <QAction>
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {

class ContactTable : public QTableWidget {
public:
    explicit ContactTable(QWidget *parent = nullptr) : QTableWidget(0, 9, parent) {
        calendar = new QCalendarWidget(this);
        calendar->setWindowFlags(Qt::ToolTip);
        calendar->setAttribute(Qt::WA_ShowWithoutActivating);
        calendar->setAttribute(Qt::WA_TransparentForMouseEvents);
        calendar->setFocusPolicy(Qt::NoFocus);
        calendar->setNavigationBarVisible(true);
        calendar->hide();
    }

protected:
    bool viewportEvent(QEvent *event) override {
        if (event->type() == QEvent::ToolTip) {
            auto *help = static_cast<QHelpEvent *>(event);
            const auto *cell = itemAt(help->pos());
            if (cell && cell->column() == 4) {
                const QDate date = cell->data(Qt::UserRole + 1).toDate();
                if (date.isValid()) {
                    calendar->setSelectedDate(date);
                    calendar->showSelectedDate();
                    calendar->adjustSize();
                    QPoint position = viewport()->mapToGlobal(visualItemRect(cell).bottomLeft());
                    if (auto *screen = QGuiApplication::screenAt(position)) {
                        const QRect area = screen->availableGeometry();
                        position.setX(qBound(area.left(), position.x(),
                                             qMax(area.left(), area.right() - calendar->width() + 1)));
                        if (position.y() + calendar->height() > area.bottom())
                            position.setY(qMax(area.top(), viewport()->mapToGlobal(
                                visualItemRect(cell).topLeft()).y() - calendar->height()));
                    }
                    calendar->move(position);
                    calendar->show();
                    event->accept();
                    return true;
                }
            }
            calendar->hide();
        } else if (event->type() == QEvent::MouseMove || event->type() == QEvent::Leave ||
                   event->type() == QEvent::MouseButtonPress || event->type() == QEvent::Wheel ||
                   event->type() == QEvent::Hide) {
            calendar->hide();
        }
        return QTableWidget::viewportEvent(event);
    }

private:
    QCalendarWidget *calendar;
};

class TextItem : public QTableWidgetItem {
public:
    explicit TextItem(const QString &text) : QTableWidgetItem(text) {}

    bool operator<(const QTableWidgetItem &other) const override {
        return QString::localeAwareCompare(text(), other.text()) < 0;
    }
};

class DateItem : public QTableWidgetItem {
public:
    explicit DateItem(const QDate &date) : QTableWidgetItem(date.toString("dd.MM.yyyy")) {
        setData(Qt::UserRole + 1, date);
    }

    bool operator<(const QTableWidgetItem &other) const override {
        return data(Qt::UserRole + 1).toDate() < other.data(Qt::UserRole + 1).toDate();
    }
};

QString phonesOfType(const Contact &contact, const QString &type) {
    QStringList numbers;
    for (const PhoneNumber &phone : contact.phones) {
        if (phone.type == type)
            numbers.append(QStringLiteral("+") + phone.digits);
    }
    return numbers.join(QStringLiteral("; "));
}

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(1200, 720);
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *layout = new QVBoxLayout(central);

    auto *fileMenu = menuBar()->addMenu(QStringLiteral("Файл"));
    QAction *newAction = fileMenu->addAction(QStringLiteral("Новый справочник"));
    QAction *openAction = fileMenu->addAction(QStringLiteral("Открыть…"));
    QAction *saveAction = fileMenu->addAction(QStringLiteral("Сохранить"));
    QAction *saveAsAction = fileMenu->addAction(QStringLiteral("Сохранить как…"));
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(QStringLiteral("Выход"));
    newAction->setShortcut(QKeySequence::New);
    openAction->setShortcut(QKeySequence::Open);
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(newAction, &QAction::triggered, this, &MainWindow::newBook);
    connect(openAction, &QAction::triggered, this, &MainWindow::openBook);
    connect(saveAction, &QAction::triggered, this, [this]() { saveBook(); });
    connect(saveAsAction, &QAction::triggered, this, [this]() { saveBookAs(); });
    connect(exitAction, &QAction::triggered, this, [this]() { close(); });

    auto *searchBox = new QGroupBox(QStringLiteral("Поиск — все заполненные условия действуют одновременно"));
    auto *searchLayout = new QGridLayout(searchBox);
    nameSearch = new QLineEdit;
    addressSearch = new QLineEdit;
    emailSearch = new QLineEdit;
    phoneSearch = new QLineEdit;
    nameSearch->setPlaceholderText(QStringLiteral("Фамилия, имя или отчество"));
    phoneSearch->setPlaceholderText(QStringLiteral("Часть номера, можно со скобками и пробелами"));
    searchLayout->addWidget(new QLabel(QStringLiteral("ФИО:")), 0, 0);
    searchLayout->addWidget(nameSearch, 0, 1);
    searchLayout->addWidget(new QLabel(QStringLiteral("Адрес:")), 0, 2);
    searchLayout->addWidget(addressSearch, 0, 3);
    searchLayout->addWidget(new QLabel(QStringLiteral("E-mail:")), 1, 0);
    searchLayout->addWidget(emailSearch, 1, 1);
    searchLayout->addWidget(new QLabel(QStringLiteral("Телефон:")), 1, 2);
    searchLayout->addWidget(phoneSearch, 1, 3);
    auto *resetButton = new QPushButton(QStringLiteral("Сбросить поиск"));
    searchLayout->addWidget(resetButton, 0, 4, 2, 1);
    layout->addWidget(searchBox);

    auto *buttons = new QHBoxLayout;
    auto *addButton = new QPushButton(QStringLiteral("Добавить контакт"));
    editButton = new QPushButton(QStringLiteral("Редактировать"));
    removeButton = new QPushButton(QStringLiteral("Удалить"));
    buttons->addWidget(addButton);
    buttons->addWidget(editButton);
    buttons->addWidget(removeButton);
    buttons->addStretch();
    layout->addLayout(buttons);

    table = new ContactTable;
    table->setHorizontalHeaderLabels({QStringLiteral("Фамилия"), QStringLiteral("Имя"),
        QStringLiteral("Отчество"), QStringLiteral("Адрес"), QStringLiteral("Дата рождения"),
        QStringLiteral("E-mail"), QStringLiteral("Рабочие телефоны"),
        QStringLiteral("Домашние телефоны"), QStringLiteral("Служебные телефоны")});
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    table->setSortingEnabled(true);
    table->setWordWrap(false);
    layout->addWidget(table);
    auto *hint = new QLabel(QStringLiteral("Сортировка: щёлкните заголовок столбца. "
                                          "Редактирование: двойной щелчок по записи."));
    layout->addWidget(hint);

    countLabel = new QLabel;
    statusBar()->addPermanentWidget(countLabel);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editContact);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeContact);
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { editContact(); });
    connect(table, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateSelectionButtons);
    // После сортировки номера строк изменяются: фильтры нужно применить заново.
    connect(table->model(), &QAbstractItemModel::layoutChanged, this, [this]() { applyFilters(); });
    for (QLineEdit *edit : {nameSearch, addressSearch, emailSearch, phoneSearch})
        connect(edit, &QLineEdit::textChanged, this, [this]() { applyFilters(); });
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        nameSearch->clear();
        addressSearch->clear();
        emailSearch->clear();
        phoneSearch->clear();
    });

    setModified(false);
    refreshTable();
}

int MainWindow::selectedContactIndex() const {
    const int row = table->currentRow();
    if (row < 0 || table->isRowHidden(row) || table->selectedItems().isEmpty())
        return -1;
    const QTableWidgetItem *item = table->item(row, 0);
    if (!item)
        return -1;
    const int index = item->data(Qt::UserRole).toInt();
    return index >= 0 && index < contacts.size() ? index : -1;
}

void MainWindow::refreshTable(int selectedIndex) {
    const int sortColumn = table->horizontalHeader()->sortIndicatorSection();
    const Qt::SortOrder order = table->horizontalHeader()->sortIndicatorOrder();
    table->setSortingEnabled(false);
    table->clearContents();
    table->setRowCount(contacts.size());
    for (int row = 0; row < contacts.size(); ++row) {
        const Contact &contact = contacts[row];
        const QStringList text = {contact.lastName, contact.firstName, contact.patronymic,
            contact.address, QString(), contact.email,
            phonesOfType(contact, phoneTypes()[0]), phonesOfType(contact, phoneTypes()[1]),
            phonesOfType(contact, phoneTypes()[2])};
        for (int column = 0; column < text.size(); ++column) {
            QTableWidgetItem *item = column == 4
                ? static_cast<QTableWidgetItem *>(new DateItem(contact.birthDate))
                : new TextItem(text[column]);
            if (column != 4)
                item->setToolTip(item->text());
            if (column == 0)
                item->setData(Qt::UserRole, row);
            table->setItem(row, column, item);
        }
    }
    table->setSortingEnabled(true);
    table->sortItems(sortColumn, order);
    table->resizeColumnsToContents();
    for (int column = 0; column < table->columnCount(); ++column)
        table->setColumnWidth(column, qBound(100, table->columnWidth(column), 260));
    applyFilters();
    table->clearSelection();
    table->setCurrentItem(nullptr);
    for (int row = 0; row < table->rowCount(); ++row) {
        if (!table->isRowHidden(row) && table->item(row, 0)->data(Qt::UserRole).toInt() == selectedIndex) {
            table->setCurrentCell(row, 0);
            table->selectRow(row);
            break;
        }
    }
    updateSelectionButtons();
}

void MainWindow::applyFilters() {
    const QStringList nameParts = nameSearch->text().simplified().split(' ', Qt::SkipEmptyParts);
    const QString address = addressSearch->text().trimmed();
    QString email = emailSearch->text();
    email.remove(QRegularExpression(QStringLiteral(R"(\s+)"),
                                    QRegularExpression::UseUnicodePropertiesOption));
    const QString phoneQuery = phoneSearch->text().trimmed();
    int visible = 0;
    for (int row = 0; row < table->rowCount(); ++row) {
        const int index = table->item(row, 0)->data(Qt::UserRole).toInt();
        const Contact &contact = contacts[index];
        const QString fullName = contact.lastName + ' ' + contact.firstName + ' ' + contact.patronymic;
        bool matches = true;
        for (const QString &part : nameParts)
            matches = matches && fullName.contains(part, Qt::CaseInsensitive);
        matches = matches && contact.address.contains(address, Qt::CaseInsensitive)
                          && contact.email.contains(email, Qt::CaseInsensitive);
        matches = matches && Validation::matchesPhoneQuery(contact.phones, phoneQuery);
        table->setRowHidden(row, !matches);
        if (matches)
            ++visible;
    }
    countLabel->setText(QStringLiteral("Показано: %1 из %2").arg(visible).arg(contacts.size()));
    updateSelectionButtons();
}

void MainWindow::updateSelectionButtons() {
    const bool selected = selectedContactIndex() >= 0;
    editButton->setEnabled(selected);
    removeButton->setEnabled(selected);
}

void MainWindow::setModified(bool value) {
    modified = value;
    const QString name = currentFile.isEmpty() ? QStringLiteral("Новый справочник")
                                              : QFileInfo(currentFile).fileName();
    setWindowTitle(QStringLiteral("%1%2 — Телефонный справочник")
                       .arg(modified ? QStringLiteral("*") : QString(), name));
}

void MainWindow::addContact() {
    ContactDialog dialog(Contact{}, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    contacts.append(dialog.contact());
    setModified(true);
    refreshTable(contacts.size() - 1);
}

void MainWindow::editContact() {
    const int index = selectedContactIndex();
    if (index == -1)
        return;
    ContactDialog dialog(contacts[index], this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    contacts[index] = dialog.contact();
    setModified(true);
    refreshTable(index);
}

void MainWindow::removeContact() {
    const int index = selectedContactIndex();
    if (index == -1)
        return;
    const Contact &contact = contacts[index];
    if (QMessageBox::question(this, QStringLiteral("Удаление контакта"),
            QStringLiteral("Удалить контакт «%1 %2»?").arg(contact.lastName, contact.firstName),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;
    contacts.removeAt(index);
    setModified(true);
    refreshTable();
}

bool MainWindow::confirmUnsavedChanges() {
    if (!modified)
        return true;
    const auto answer = QMessageBox::question(this, QStringLiteral("Несохранённые изменения"),
        QStringLiteral("Сохранить изменения в справочнике?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if (answer == QMessageBox::Save)
        return saveBook();
    return answer == QMessageBox::Discard;
}

void MainWindow::newBook() {
    if (!confirmUnsavedChanges())
        return;
    contacts.clear();
    currentFile.clear();
    setModified(false);
    refreshTable();
}

void MainWindow::openBook() {
    const QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Открыть справочник"),
        QString(), QStringLiteral("Справочник JSON (*.json);;Все файлы (*)"));
    if (fileName.isEmpty() || !confirmUnsavedChanges())
        return;
    QVector<Contact> loaded;
    QString error;
    if (!ContactStore::load(fileName, loaded, error)) {
        QMessageBox::critical(this, QStringLiteral("Не удалось открыть файл"), error);
        return;
    }
    contacts = loaded;
    currentFile = fileName;
    setModified(false);
    refreshTable();
    statusBar()->showMessage(QStringLiteral("Справочник загружен"), 4000);
}

bool MainWindow::writeBook(const QString &fileName) {
    QString error;
    if (!ContactStore::save(fileName, contacts, error)) {
        QMessageBox::critical(this, QStringLiteral("Не удалось сохранить файл"), error);
        return false;
    }
    currentFile = fileName;
    setModified(false);
    statusBar()->showMessage(QStringLiteral("Справочник сохранён"), 4000);
    return true;
}

bool MainWindow::saveBook() {
    return currentFile.isEmpty() ? saveBookAs() : writeBook(currentFile);
}

bool MainWindow::saveBookAs() {
    QFileDialog dialog(this, QStringLiteral("Сохранить справочник"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(QStringLiteral("Справочник JSON (*.json);;Все файлы (*)"));
    dialog.setDefaultSuffix(QStringLiteral("json"));
    dialog.selectFile(currentFile.isEmpty() ? QStringLiteral("contacts.json") : currentFile);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return writeBook(dialog.selectedFiles().first());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (confirmUnsavedChanges())
        event->accept();
    else
        event->ignore();
}
