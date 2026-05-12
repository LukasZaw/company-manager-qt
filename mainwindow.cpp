#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "src/services/employeeservice.h"
#include "src/ui/employeedialog.h"
#include "src/ui/departmentdialog.h"
#include <QHeaderView>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    employeeModel = new EmployeeTableModel(this);

    ui->employeesTableView->setModel(employeeModel);

    ui->employeesTableView
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
            );

    ui->employeesTableView
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
            );

    ui->employeesTableView
        ->setSortingEnabled(true);

    ui->employeesTableView
        ->horizontalHeader()
        ->setStretchLastSection(true);

    loadEmployees();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadEmployees()
{
    auto employees =
        EmployeeService::getAllEmployees();

    employeeModel
        ->setEmployees(employees);
}

void MainWindow::on_addEmployeeButton_clicked()
{
    EmployeeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        Employee employee = dialog.getEmployee();

        EmployeeService::addEmployee(employee);

        loadEmployees();
    }

}

void MainWindow::on_deleteEmployeeButton_clicked()
{
    QModelIndex index =
        ui->employeesTableView
            ->currentIndex();

    if (!index.isValid())
        return;

    Employee employee = employeeModel->getEmployee(index.row());

    EmployeeService::deleteEmployee(employee.id);

    loadEmployees();
}

void MainWindow::on_editEmployeeButton_clicked()
{
    const QModelIndex index = ui->employeesTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz pracownika z listy."));
        return;
    }

    editEmployeeAtRow(index.row());
}

void MainWindow::on_employeesTableView_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    editEmployeeAtRow(index.row());
}

void MainWindow::editEmployeeAtRow(int row)
{
    const Employee employee = employeeModel->getEmployee(row);
    if (employee.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się odczytać pracownika."));
        return;
    }

    EmployeeDialog dialog(this);
    dialog.setEmployee(employee);

    if (dialog.exec() == QDialog::Accepted) {
        const Employee updated = dialog.getEmployee();
        if (!EmployeeService::updateEmployee(updated)) {
            QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zapisać zmian."));
            return;
        }

        loadEmployees();
    }
}

void MainWindow::on_manageDepartmentsButton_clicked()
{
    on_actionDepartments_triggered();
}

void MainWindow::on_actionDepartments_triggered()
{
    DepartmentDialog dialog(this);
    dialog.exec();
}

