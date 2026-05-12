#include "databasemanager.h"
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

    // Enforce foreign keys in SQLite
    {
        QSqlQuery pragmaQuery;
        if (!pragmaQuery.exec("PRAGMA foreign_keys = ON;")) {
            qDebug() << "PRAGMA foreign_keys ERROR:" << pragmaQuery.lastError().text();
        }
    }

    initialize();
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() {
    return db;
}

void DatabaseManager::initialize() {
    QSqlQuery query;

    const QString createDepartments =
        "CREATE TABLE IF NOT EXISTS departments ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE"
        ");";

    if (!query.exec(createDepartments)) {
        qDebug() << "CREATE TABLE departments ERROR:" << query.lastError().text();
        return;
    }

    // Ensure at least one department exists (minimal default)
    if (!query.exec("INSERT OR IGNORE INTO departments (name) VALUES ('Ogólny');")) {
        qDebug() << "INSERT default department ERROR:" << query.lastError().text();
    }

    const QString createEmployees =
        "CREATE TABLE IF NOT EXISTS employees ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "email TEXT,"
        "phone TEXT,"
        "position TEXT,"
        "department_id INTEGER NOT NULL,"
        "hire_date TEXT,"
        "status INTEGER DEFAULT 1,"
        "notes TEXT,"
        "FOREIGN KEY(department_id) REFERENCES departments(id) ON UPDATE CASCADE ON DELETE RESTRICT"
        ");";

    if (!query.exec(createEmployees)) {
        qDebug() << "CREATE TABLE ERROR:" << query.lastError().text();
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_employees_department_id ON employees(department_id);")) {
        qDebug() << "CREATE INDEX ERROR:" << query.lastError().text();
    }
}
