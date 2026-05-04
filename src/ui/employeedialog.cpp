#include "employeedialog.h"
#include "ui_employeedialog.h"

EmployeeDialog::EmployeeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EmployeeDialog)
{
    ui->setupUi(this);
}

EmployeeDialog::~EmployeeDialog()
{
    delete ui;
}

Employee EmployeeDialog::getEmployee() const {

    Employee employee;

    employee.firstName = ui->firstNameEdit->text();
    employee.lastName = ui->lastNameEdit->text();

    return employee;
}

void EmployeeDialog::on_saveButton_clicked()
{
    accept();
}

