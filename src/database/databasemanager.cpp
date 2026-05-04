#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QSqlDatabase DatabaseManager::db;

bool DatabaseManager::connect() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("company.db");
    }

    if (!db.open()) {
        qDebug() << "DB ERROR:" << db.lastError().text();
        return false;
    }

    initialize();
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() {
    return db;
}

void DatabaseManager::initialize() {
    QSqlQuery query;

    QString createEmployees =
        "CREATE TABLE IF NOT EXISTS employees ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL"
        ");";

    if (!query.exec(createEmployees)) {
        qDebug() << "CREATE TABLE ERROR:" << query.lastError().text();
    }


}
