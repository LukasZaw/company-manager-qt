#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "src/services/employeeservice.h"
#include "src/ui/employeedialog.h"
#include "src/ui/departmentdialog.h"
#include <QHeaderView>



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


void MainWindow::on_pushButton_2_clicked()
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

void MainWindow::on_actionDepartments_triggered()
{
    DepartmentDialog dialog(this);
    dialog.exec();
}

