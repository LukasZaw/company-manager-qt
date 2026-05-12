#include "departmentdialog.h"
#include "ui_departmentdialog.h"

#include "../services/departmentservice.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

DepartmentDialog::DepartmentDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DepartmentDialog)
    , m_model(new DepartmentTableModel(this))
{
    ui->setupUi(this);

    ui->departmentsTableView->setModel(m_model);
    ui->departmentsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->departmentsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->departmentsTableView->setSortingEnabled(true);
    ui->departmentsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->departmentsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadDepartments();
}

DepartmentDialog::~DepartmentDialog() {
    delete ui;
}

void DepartmentDialog::loadDepartments() {
    const auto deps = DepartmentService::getAllDepartments();
    m_model->setDepartments(deps);
}

void DepartmentDialog::on_addDepartmentButton_clicked() {
    const QString name = ui->departmentNameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Podaj nazwę działu."));
        return;
    }

    if (!DepartmentService::addDepartment(name)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się dodać działu. Możliwe, że nazwa już istnieje."));
        return;
    }

    ui->departmentNameEdit->clear();
    loadDepartments();
}

void DepartmentDialog::on_refreshButton_clicked() {
    loadDepartments();
}

void DepartmentDialog::on_renameButton_clicked() {
    const QModelIndex index = ui->departmentsTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz dział z listy."));
        return;
    }

    const Department department = m_model->getDepartment(index.row());
    if (department.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nieprawidłowy dział."));
        return;
    }

    bool ok = false;
    const QString newName = QInputDialog::getText(
        this,
        tr("Zmień nazwę działu"),
        tr("Nowa nazwa:"),
        QLineEdit::Normal,
        department.name,
        &ok
    );

    if (!ok)
        return;

    if (newName.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nazwa działu nie może być pusta."));
        return;
    }

    if (!DepartmentService::updateDepartment(department.id, newName)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zmienić nazwy działu. Możliwe, że nazwa już istnieje."));
        return;
    }

    loadDepartments();
}

void DepartmentDialog::on_deleteButton_clicked() {
    const QModelIndex index = ui->departmentsTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz dział z listy."));
        return;
    }

    const Department department = m_model->getDepartment(index.row());
    if (department.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nieprawidłowy dział."));
        return;
    }

    const auto reply = QMessageBox::question(
        this,
        tr("Potwierdzenie"),
        tr("Usunąć dział '%1'?" ).arg(department.name),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!DepartmentService::deleteDepartment(department.id, &errorMessage)) {
        QMessageBox::warning(this, tr("Błąd"), errorMessage.isEmpty() ? tr("Nie udało się usunąć działu.") : errorMessage);
        return;
    }

    loadDepartments();
}

void DepartmentDialog::on_closeButton_clicked() {
    reject();
}
