#include "departmentdialog.h"
#include "ui_departmentdialog.h"

#include "../services/departmentservice.h"

#include <QHeaderView>
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

void DepartmentDialog::on_closeButton_clicked() {
    reject();
}
