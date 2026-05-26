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

    // Enforce foreign keys
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

    // Ensure one category exists
    if (!query.exec("INSERT OR IGNORE INTO categories (name) VALUES ('Ogólna');")) {
        qDebug() << "INSERT default category ERROR:" << query.lastError().text();
    }

    const QString createLocations =
        "CREATE TABLE IF NOT EXISTS locations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "parent_id INTEGER,"
        "name TEXT NOT NULL,"
        "path TEXT NOT NULL UNIQUE,"
        "FOREIGN KEY(parent_id) REFERENCES locations(id) ON UPDATE CASCADE ON DELETE RESTRICT"
        ");";

    if (!query.exec(createLocations)) {
        qDebug() << "CREATE TABLE locations ERROR:" << query.lastError().text();
        return;
    }

    if (!query.exec("CREATE UNIQUE INDEX IF NOT EXISTS ux_locations_parent_name ON locations(parent_id, name);")) {
        qDebug() << "CREATE INDEX locations ERROR:" << query.lastError().text();
    }

    if (!query.exec("INSERT OR IGNORE INTO locations (id, parent_id, name, path) VALUES (1, NULL, 'Magazyn', 'Magazyn');")) {
        qDebug() << "INSERT default location ERROR:" << query.lastError().text();
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
        "location_id INTEGER,"
        "description TEXT,"
        "FOREIGN KEY(category_id) REFERENCES categories(id) ON UPDATE CASCADE ON DELETE RESTRICT,"
        "FOREIGN KEY(location_id) REFERENCES locations(id) ON UPDATE CASCADE ON DELETE SET NULL"
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
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_products_location_id ON products(location_id);")) {
        qDebug() << "CREATE INDEX products ERROR:" << query.lastError().text();
    }

    const QString createWarehouseMovements =
        "CREATE TABLE IF NOT EXISTS warehouse_movements ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "type TEXT NOT NULL CHECK (type IN ('RECEIPT','ISSUE','RELOCATE','ADJUST')),"
        "occurred_at TEXT NOT NULL,"
        "employee_id INTEGER,"
        "from_location_id INTEGER,"
        "to_location_id INTEGER,"
        "notes TEXT,"
        "is_canceled INTEGER NOT NULL DEFAULT 0 CHECK (is_canceled IN (0,1)),"
        "affects_stock INTEGER NOT NULL DEFAULT 1 CHECK (affects_stock IN (0,1)),"
        "FOREIGN KEY(employee_id) REFERENCES employees(id) ON UPDATE CASCADE ON DELETE SET NULL,"
        "FOREIGN KEY(from_location_id) REFERENCES locations(id) ON UPDATE CASCADE ON DELETE SET NULL,"
        "FOREIGN KEY(to_location_id) REFERENCES locations(id) ON UPDATE CASCADE ON DELETE SET NULL"
        ");";

    if (!query.exec(createWarehouseMovements)) {
        qDebug() << "CREATE TABLE warehouse_movements ERROR:" << query.lastError().text();
        return;
    }

    const QString createWarehouseMovementLines =
        "CREATE TABLE IF NOT EXISTS warehouse_movement_lines ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "movement_id INTEGER NOT NULL,"
        "product_id INTEGER NOT NULL,"
        "quantity REAL NOT NULL CHECK (quantity != 0),"
        "FOREIGN KEY(movement_id) REFERENCES warehouse_movements(id) ON UPDATE CASCADE ON DELETE CASCADE,"
        "FOREIGN KEY(product_id) REFERENCES products(id) ON UPDATE CASCADE ON DELETE RESTRICT"
        ");";

    if (!query.exec(createWarehouseMovementLines)) {
        qDebug() << "CREATE TABLE warehouse_movement_lines ERROR:" << query.lastError().text();
        return;
    }

    // indexes on warehouse movements
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movements_occurred_at ON warehouse_movements(occurred_at);")) {
        qDebug() << "CREATE INDEX warehouse_movements ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movements_type ON warehouse_movements(type);")) {
        qDebug() << "CREATE INDEX warehouse_movements ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movements_employee_id ON warehouse_movements(employee_id);")) {
        qDebug() << "CREATE INDEX warehouse_movements ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movements_from_location_id ON warehouse_movements(from_location_id);")) {
        qDebug() << "CREATE INDEX warehouse_movements ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movements_to_location_id ON warehouse_movements(to_location_id);")) {
        qDebug() << "CREATE INDEX warehouse_movements ERROR:" << query.lastError().text();
    }

    // indexes on movement lines
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movement_lines_movement_id ON warehouse_movement_lines(movement_id);")) {
        qDebug() << "CREATE INDEX warehouse_movement_lines ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_warehouse_movement_lines_product_id ON warehouse_movement_lines(product_id);")) {
        qDebug() << "CREATE INDEX warehouse_movement_lines ERROR:" << query.lastError().text();
    }
}
