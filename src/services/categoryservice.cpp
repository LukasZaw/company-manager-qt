#include "categoryservice.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

QList<Category> CategoryService::getAllCategories()
{
    QList<Category> list;

    QSqlQuery query;
    if (!query.exec("SELECT id, name FROM categories ORDER BY name")) {
        qDebug() << "SELECT categories ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        Category c;
        c.id = query.value("id").toInt();
        c.name = query.value("name").toString();
        list.append(c);
    }

    return list;
}

bool CategoryService::addCategory(const QString& name)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty())
        return false;

    QSqlQuery query;
    query.prepare("INSERT INTO categories (name) VALUES (:name)");
    query.bindValue(":name", trimmed);

    if (!query.exec()) {
        qDebug() << "INSERT category ERROR:" << query.lastError().text();
        return false;
    }

    return true;
}

bool CategoryService::updateCategory(int id, const QString& name)
{
    const QString trimmed = name.trimmed();
    if (id <= 0 || trimmed.isEmpty())
        return false;

    QSqlQuery query;
    query.prepare("UPDATE categories SET name = :name WHERE id = :id");
    query.bindValue(":name", trimmed);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "UPDATE category ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool CategoryService::deleteCategory(int id, QString* errorMessage)
{
    if (id <= 0) {
        if (errorMessage)
            *errorMessage = QObject::tr("Nieprawidłowy identyfikator kategorii.");
        return false;
    }

    // Guard: block delete if category is used by any product
    {
        QSqlQuery check;
        check.prepare("SELECT COUNT(1) FROM products WHERE category_id = :id");
        check.bindValue(":id", id);
        if (!check.exec()) {
            qDebug() << "CHECK category usage ERROR:" << check.lastError().text();
            if (errorMessage)
                *errorMessage = QObject::tr("Nie udało się sprawdzić użycia kategorii.");
            return false;
        }
        if (check.next() && check.value(0).toInt() > 0) {
            if (errorMessage)
                *errorMessage = QObject::tr("Nie można usunąć kategorii, ponieważ jest przypisana do produktów.");
            return false;
        }
    }

    QSqlQuery query;
    query.prepare("DELETE FROM categories WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "DELETE category ERROR:" << query.lastError().text();
        if (errorMessage)
            *errorMessage = QObject::tr("Nie udało się usunąć kategorii.");
        return false;
    }

    return query.numRowsAffected() > 0;
}
