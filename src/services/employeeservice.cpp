#include "employeeservice.h"

#include <QSqlQuery>
#include <QSqlError>

QList<Employee> EmployeeService::getAllEmployees(){
    QList<Employee> list;

    QSqlQuery query("SELECT id, first_name, last_name FROM employees");

    while (query.next()) {
        Employee e;

        e.id = query.value("id").toInt();
        e.firstName = query.value("first_name").toString();
        e.lastName = query.value("last_name").toString();

        list.append(e);
    }

    return list;
}

bool EmployeeService::addEmployee(const Employee& employee) {
    QSqlQuery query;

    query.prepare("INSERT INTO employees (first_name, last_name) VALUES (:first_name, :last_name)");

    query.bindValue(":first_name", employee.firstName);
    query.bindValue(":last_name", employee.lastName);

    if (!query.exec()) {
        qDebug() << "INSERT ERROR:" << query.lastError().text();
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
