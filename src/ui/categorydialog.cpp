#include "categorydialog.h"
#include "ui_categorydialog.h"

#include "../services/categoryservice.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

CategoryDialog::CategoryDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::CategoryDialog)
    , m_model(new CategoryTableModel(this))
{
    ui->setupUi(this);

    ui->categoriesTableView->setModel(m_model);
    ui->categoriesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->categoriesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->categoriesTableView->setSortingEnabled(true);
    ui->categoriesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->categoriesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadCategories();
}

CategoryDialog::~CategoryDialog()
{
    delete ui;
}

void CategoryDialog::loadCategories()
{
    const auto categories = CategoryService::getAllCategories();
    m_model->setCategories(categories);
}

void CategoryDialog::on_addCategoryButton_clicked()
{
    const QString name = ui->categoryNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Podaj nazwę kategorii."));
        return;
    }

    if (!CategoryService::addCategory(name)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się dodać kategorii. Możliwe, że nazwa już istnieje."));
        return;
    }

    ui->categoryNameEdit->clear();
    loadCategories();
}

void CategoryDialog::on_refreshButton_clicked()
{
    loadCategories();
}

void CategoryDialog::on_renameButton_clicked()
{
    const QModelIndex index = ui->categoriesTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz kategorię z listy."));
        return;
    }

    const Category category = m_model->getCategory(index.row());
    if (category.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nieprawidłowa kategoria."));
        return;
    }

    bool ok = false;
    const QString newName = QInputDialog::getText(
        this,
        tr("Zmień nazwę kategorii"),
        tr("Nowa nazwa:"),
        QLineEdit::Normal,
        category.name,
        &ok);

    if (!ok)
        return;

    if (newName.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nazwa kategorii nie może być pusta."));
        return;
    }

    if (!CategoryService::updateCategory(category.id, newName)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zmienić nazwy kategorii. Możliwe, że nazwa już istnieje."));
        return;
    }

    loadCategories();
}

void CategoryDialog::on_deleteButton_clicked()
{
    const QModelIndex index = ui->categoriesTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz kategorię z listy."));
        return;
    }

    const Category category = m_model->getCategory(index.row());
    if (category.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nieprawidłowa kategoria."));
        return;
    }

    const auto reply = QMessageBox::question(
        this,
        tr("Potwierdzenie"),
        tr("Usunąć kategorię '%1'?" ).arg(category.name),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!CategoryService::deleteCategory(category.id, &errorMessage)) {
        QMessageBox::warning(this, tr("Błąd"), errorMessage.isEmpty() ? tr("Nie udało się usunąć kategorii.") : errorMessage);
        return;
    }

    loadCategories();
}

void CategoryDialog::on_closeButton_clicked()
{
    reject();
}
