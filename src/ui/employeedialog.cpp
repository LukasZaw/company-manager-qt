#include "employeedialog.h"
#include "ui_employeedialog.h"

#include "../services/departmentservice.h"

#include <QMessageBox>
#include <QDate>

EmployeeDialog::EmployeeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EmployeeDialog)
{
    ui->setupUi(this);

    // Populate departments
    const auto departments = DepartmentService::getAllDepartments();
    ui->departmentComboBox->clear();
    for (const auto& d : departments) {
        ui->departmentComboBox->addItem(d.name, d.id);
    }

    // Defaults
    ui->statusCheckBox->setChecked(true);
}

EmployeeDialog::~EmployeeDialog()
{
    delete ui;
}

Employee EmployeeDialog::getEmployee() const {

    Employee employee;

    employee.id = m_employeeId;

    employee.firstName = ui->firstNameEdit->text();
    employee.lastName = ui->lastNameEdit->text();
    employee.email = ui->emailEdit->text();
    employee.phone = ui->phoneEdit->text();
    employee.position = ui->positionEdit->text();
    employee.departmentId = ui->departmentComboBox->currentData().toInt();
    employee.department = ui->departmentComboBox->currentText();
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

    if (ui->departmentComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Wybierz dział."));
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
    m_employeeId = employee.id;
    ui->firstNameEdit->setText(employee.firstName);
    ui->lastNameEdit->setText(employee.lastName);
    ui->emailEdit->setText(employee.email);
    ui->phoneEdit->setText(employee.phone);
    ui->positionEdit->setText(employee.position);

    // Select by id if possible
    int idx = -1;
    for (int i = 0; i < ui->departmentComboBox->count(); ++i) {
        if (ui->departmentComboBox->itemData(i).toInt() == employee.departmentId) {
            idx = i;
            break;
        }
    }
    if (idx >= 0)
        ui->departmentComboBox->setCurrentIndex(idx);

    if (employee.hireDate.isValid())
        ui->hireDateEdit->setDate(employee.hireDate);
    ui->statusCheckBox->setChecked(employee.active);
    ui->notesEdit->setPlainText(employee.notes);
}

