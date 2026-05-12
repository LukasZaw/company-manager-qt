#include "employeedialog.h"
#include "ui_employeedialog.h"

#include <QMessageBox>
#include <QDate>

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
    employee.email = ui->emailEdit->text();
    employee.phone = ui->phoneEdit->text();
    employee.position = ui->positionEdit->text();
    employee.department = ui->departmentEdit->text();
    employee.hireDate = ui->hireDateEdit->date();
    employee.active = ui->statusCheckBox->isChecked();
    employee.notes = ui->notesEdit->toPlainText();

    return employee;
}

void EmployeeDialog::on_saveButton_clicked()
{
    // Basic validation
    if (ui->firstNameEdit->text().trimmed().isEmpty() || ui->lastNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Imię i nazwisko są wymagane."));
        return;
    }

    const QString email = ui->emailEdit->text().trimmed();
    if (!email.isEmpty() && !email.contains('@')) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nieprawidłowy adres e-mail."));
        return;
    }

    accept();
}

void EmployeeDialog::setEmployee(const Employee& employee) {
    ui->firstNameEdit->setText(employee.firstName);
    ui->lastNameEdit->setText(employee.lastName);
    ui->emailEdit->setText(employee.email);
    ui->phoneEdit->setText(employee.phone);
    ui->positionEdit->setText(employee.position);
    ui->departmentEdit->setText(employee.department);
    if (employee.hireDate.isValid())
        ui->hireDateEdit->setDate(employee.hireDate);
    ui->statusCheckBox->setChecked(employee.active);
    ui->notesEdit->setPlainText(employee.notes);
}

