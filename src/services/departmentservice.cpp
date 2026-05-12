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
