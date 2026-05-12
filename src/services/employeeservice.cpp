#include "employeeservice.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

QList<Employee> EmployeeService::getAllEmployees(){
    QList<Employee> list;

    QSqlQuery query;
    const QString sql = QStringLiteral(
        "SELECT id, first_name, last_name, email, phone, position, department, hire_date, status, notes FROM employees"
    );

    if (!query.exec(sql)) {
        qDebug() << "SELECT ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        Employee e;

        e.id = query.value("id").toInt();
        e.firstName = query.value("first_name").toString();
        e.lastName = query.value("last_name").toString();
        e.email = query.value("email").toString();
        e.phone = query.value("phone").toString();
        e.position = query.value("position").toString();
        e.department = query.value("department").toString();
        e.hireDate = QDate::fromString(query.value("hire_date").toString(), Qt::ISODate);
        e.active = query.value("status").toInt() != 0;
        e.notes = query.value("notes").toString();

        list.append(e);
    }

    return list;
}

Employee EmployeeService::getEmployeeById(int id) {
    Employee e;
    QSqlQuery query;
    query.prepare("SELECT id, first_name, last_name, email, phone, position, department, hire_date, status, notes FROM employees WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SELECT BY ID ERROR:" << query.lastError().text();
        return e;
    }

    if (query.next()) {
        e.id = query.value("id").toInt();
        e.firstName = query.value("first_name").toString();
        e.lastName = query.value("last_name").toString();
        e.email = query.value("email").toString();
        e.phone = query.value("phone").toString();
        e.position = query.value("position").toString();
        e.department = query.value("department").toString();
        e.hireDate = QDate::fromString(query.value("hire_date").toString(), Qt::ISODate);
        e.active = query.value("status").toInt() != 0;
        e.notes = query.value("notes").toString();
    }

    return e;
}

bool EmployeeService::addEmployee(const Employee& employee) {
    QSqlQuery query;

    query.prepare("INSERT INTO employees (first_name, last_name, email, phone, position, department, hire_date, status, notes) VALUES (:first_name, :last_name, :email, :phone, :position, :department, :hire_date, :status, :notes)");

    query.bindValue(":first_name", employee.firstName);
    query.bindValue(":last_name", employee.lastName);
    query.bindValue(":email", employee.email);
    query.bindValue(":phone", employee.phone);
    query.bindValue(":position", employee.position);
    query.bindValue(":department", employee.department);
    query.bindValue(":hire_date", employee.hireDate.isValid() ? employee.hireDate.toString(Qt::ISODate) : QString());
    query.bindValue(":status", employee.active ? 1 : 0);
    query.bindValue(":notes", employee.notes);

    if (!query.exec()) {
        qDebug() << "INSERT ERROR:" << query.lastError().text();
        return false;
    }

    return true;
}

bool EmployeeService::updateEmployee(const Employee& employee) {
    QSqlQuery query;

    query.prepare("UPDATE employees SET first_name = :first_name, last_name = :last_name, email = :email, phone = :phone, position = :position, department = :department, hire_date = :hire_date, status = :status, notes = :notes WHERE id = :id");

    query.bindValue(":first_name", employee.firstName);
    query.bindValue(":last_name", employee.lastName);
    query.bindValue(":email", employee.email);
    query.bindValue(":phone", employee.phone);
    query.bindValue(":position", employee.position);
    query.bindValue(":department", employee.department);
    query.bindValue(":hire_date", employee.hireDate.isValid() ? employee.hireDate.toString(Qt::ISODate) : QString());
    query.bindValue(":status", employee.active ? 1 : 0);
    query.bindValue(":notes", employee.notes);
    query.bindValue(":id", employee.id);

    if (!query.exec()) {
        qDebug() << "UPDATE ERROR:" << query.lastError().text();
        return false;
    }

    return true;
}

bool EmployeeService::deleteEmployee(int id) {
    QSqlQuery query;

    query.prepare("DELETE FROM employees WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "DELETE ERROR:" << query.lastError().text();
        return false;
    }

    return true;
}
