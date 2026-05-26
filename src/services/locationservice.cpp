#include "locationservice.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

QString LocationService::sanitizeSegment(const QString& name)
{
    QString s = name.trimmed();
    s.replace("/", "-");
    s.replace("\\", "-");
    return s;
}

QList<Location> LocationService::getAllLocations()
{
    QList<Location> list;

    QSqlQuery query;
    if (!query.exec("SELECT id, COALESCE(parent_id, 0) AS parent_id, name, path FROM locations ORDER BY path")) {
        qDebug() << "SELECT locations ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        Location l;
        l.id = query.value("id").toInt();
        l.parentId = query.value("parent_id").toInt();
        l.name = query.value("name").toString();
        l.path = query.value("path").toString();
        list.append(l);
    }

    return list;
}

Location LocationService::getLocationById(int id)
{
    Location l;
    if (id <= 0)
        return l;

    QSqlQuery query;
    query.prepare("SELECT id, COALESCE(parent_id, 0) AS parent_id, name, path FROM locations WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SELECT location by id ERROR:" << query.lastError().text();
        return l;
    }

    if (!query.next())
        return l;

    l.id = query.value("id").toInt();
    l.parentId = query.value("parent_id").toInt();
    l.name = query.value("name").toString();
    l.path = query.value("path").toString();
    return l;
}

QString LocationService::getPathById(int id)
{
    if (id <= 0)
        return QString();

    QSqlQuery query;
    query.prepare("SELECT path FROM locations WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SELECT location path ERROR:" << query.lastError().text();
        return QString();
    }

    if (!query.next())
        return QString();

    return query.value(0).toString();
}

bool LocationService::addLocation(int parentId, const QString& name, int* outId)
{
    if (outId)
        *outId = 0;

    const QString seg = sanitizeSegment(name);
    if (seg.isEmpty())
        return false;

    QString parentPath;
    if (parentId > 0) {
        parentPath = getPathById(parentId);
        if (parentPath.isEmpty())
            return false;
    }

    const QString path = parentPath.isEmpty() ? seg : (parentPath + "/" + seg);

    QSqlQuery query;
    query.prepare("INSERT INTO locations (parent_id, name, path) VALUES (:parent_id, :name, :path)");

    if (parentId > 0)
        query.bindValue(":parent_id", parentId);
    else
        query.bindValue(":parent_id", QVariant(QVariant::Int));

    query.bindValue(":name", seg);
    query.bindValue(":path", path);

    if (!query.exec()) {
        qDebug() << "INSERT location ERROR:" << query.lastError().text();
        return false;
    }

    const int newId = query.lastInsertId().toInt();
    if (outId)
        *outId = newId;

    return newId > 0;
}

bool LocationService::renameLocation(int id, const QString& newName)
{
    if (id <= 0)
        return false;

    const QString seg = sanitizeSegment(newName);
    if (seg.isEmpty())
        return false;

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen())
        return false;

    int parentId = 0;
    QString oldPath;

    {
        QSqlQuery q(db);
        q.prepare("SELECT COALESCE(parent_id, 0) AS parent_id, path FROM locations WHERE id = :id");
        q.bindValue(":id", id);
        if (!q.exec()) {
            qDebug() << "SELECT location for rename ERROR:" << q.lastError().text();
            return false;
        }
        if (!q.next())
            return false;

        parentId = q.value("parent_id").toInt();
        oldPath = q.value("path").toString();
        if (oldPath.isEmpty())
            return false;
    }

    QString parentPath;
    if (parentId > 0)
        parentPath = getPathById(parentId);

    const QString newPath = parentPath.isEmpty() ? seg : (parentPath + "/" + seg);

    if (!db.transaction()) {
        qDebug() << "DB transaction ERROR:" << db.lastError().text();
        return false;
    }

    {
        QSqlQuery q(db);
        q.prepare("UPDATE locations SET name = :name, path = :path WHERE id = :id");
        q.bindValue(":name", seg);
        q.bindValue(":path", newPath);
        q.bindValue(":id", id);
        if (!q.exec()) {
            qDebug() << "UPDATE location rename ERROR:" << q.lastError().text();
            db.rollback();
            return false;
        }
        if (q.numRowsAffected() <= 0) {
            db.rollback();
            return false;
        }
    }

    {
        QSqlQuery q(db);
        q.prepare(
            "UPDATE locations "
            "SET path = :newPrefix || substr(path, length(:oldPrefix) + 1) "
            "WHERE path LIKE :oldPrefix || '/%'");
        q.bindValue(":newPrefix", newPath);
        q.bindValue(":oldPrefix", oldPath);
        if (!q.exec()) {
            qDebug() << "UPDATE subtree paths ERROR:" << q.lastError().text();
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        qDebug() << "DB commit ERROR:" << db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}

bool LocationService::deleteLocation(int id)
{
    if (id <= 0)
        return false;

    QSqlQuery query;
    query.prepare("DELETE FROM locations WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "DELETE location ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
