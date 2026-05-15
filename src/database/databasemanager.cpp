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

    const QString createCategories =
        "CREATE TABLE IF NOT EXISTS categories ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL UNIQUE"
        ");";

    if (!query.exec(createCategories)) {
        qDebug() << "CREATE TABLE categories ERROR:" << query.lastError().text();
        return;
    }

    // Ensure at least one category exists (minimal default)
    if (!query.exec("INSERT OR IGNORE INTO categories (name) VALUES ('Ogólna');")) {
        qDebug() << "INSERT default category ERROR:" << query.lastError().text();
    }

    const QString createProducts =
        "CREATE TABLE IF NOT EXISTS products ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "sku TEXT NOT NULL UNIQUE,"
        "category_id INTEGER NOT NULL,"
        "price REAL NOT NULL DEFAULT 0,"
        "quantity REAL NOT NULL DEFAULT 0,"
        "unit TEXT,"
        "location TEXT,"
        "description TEXT,"
        "FOREIGN KEY(category_id) REFERENCES categories(id) ON UPDATE CASCADE ON DELETE RESTRICT"
        ");";

    if (!query.exec(createProducts)) {
        qDebug() << "CREATE TABLE products ERROR:" << query.lastError().text();
        return;
    }

    // indexes on products
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_products_sku ON products(sku);")) {
        qDebug() << "CREATE INDEX products ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_products_category_id ON products(category_id);")) {
        qDebug() << "CREATE INDEX products ERROR:" << query.lastError().text();
    }
}
