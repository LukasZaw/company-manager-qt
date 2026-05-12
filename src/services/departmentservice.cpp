#include "departmentservice.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

QList<Department> DepartmentService::getAllDepartments() {
    QList<Department> list;

    QSqlQuery query;
    if (!query.exec("SELECT id, name FROM departments ORDER BY name")) {
        qDebug() << "SELECT departments ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        Department d;
        d.id = query.value("id").toInt();
        d.name = query.value("name").toString();
        list.append(d);
    }

    return list;
}

bool DepartmentService::addDepartment(const QString& name) {
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO departments (name) VALUES (:name)");
    query.bindValue(":name", trimmed);

    if (!query.exec()) {
        qDebug() << "INSERT department ERROR:" << query.lastError().text();
        return false;
    }

    return true;
}
