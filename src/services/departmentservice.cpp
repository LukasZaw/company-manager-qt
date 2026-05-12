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

bool DepartmentService::updateDepartment(int id, const QString& name) {
    const QString trimmed = name.trimmed();
    if (id <= 0 || trimmed.isEmpty()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE departments SET name = :name WHERE id = :id");
    query.bindValue(":name", trimmed);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "UPDATE department ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DepartmentService::deleteDepartment(int id, QString* errorMessage) {
    if (id <= 0) {
        if (errorMessage)
            *errorMessage = QObject::tr("Nieprawidłowy identyfikator działu.");
        return false;
    }

    // Guard: block delete if department is used by any employee
    {
        QSqlQuery check;
        check.prepare("SELECT COUNT(1) FROM employees WHERE department_id = :id");
        check.bindValue(":id", id);
        if (!check.exec()) {
            qDebug() << "CHECK department usage ERROR:" << check.lastError().text();
            if (errorMessage)
                *errorMessage = QObject::tr("Nie udało się sprawdzić użycia działu.");
            return false;
        }
        if (check.next() && check.value(0).toInt() > 0) {
            if (errorMessage)
                *errorMessage = QObject::tr("Nie można usunąć działu, ponieważ jest przypisany do pracowników.");
            return false;
        }
    }

    QSqlQuery query;
    query.prepare("DELETE FROM departments WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "DELETE department ERROR:" << query.lastError().text();
        if (errorMessage)
            *errorMessage = QObject::tr("Nie udało się usunąć działu.");
        return false;
    }

    return query.numRowsAffected() > 0;
}
