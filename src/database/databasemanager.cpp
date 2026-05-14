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

    auto tableHasColumn = [](const QString& table, const QString& column) -> bool {
        QSqlQuery q;
        q.prepare("PRAGMA table_info(" + table + ")");
        if (!q.exec())
            return false;

        while (q.next()) {
            const QString name = q.value("name").toString();
            if (name.compare(column, Qt::CaseInsensitive) == 0)
                return true;
        }
        return false;
    };

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

    const QString createProductsV2 =
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

    // If products exists in older form (category TEXT), migrate by rebuilding the table.
    if (!tableHasColumn("products", "category_id")) {
        // Try to detect old schema; if products table doesn't exist, create v2 directly.
        QSqlQuery existsQ;
        existsQ.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='products'");
        const bool productsExists = existsQ.exec() && existsQ.next();

        if (!productsExists) {
            if (!query.exec(createProductsV2)) {
                qDebug() << "CREATE TABLE products ERROR:" << query.lastError().text();
                return;
            }
        } else {
            // Migrate: products(category TEXT) -> products(category_id INTEGER)
            if (!query.exec("BEGIN")) {
                qDebug() << "BEGIN migration ERROR:" << query.lastError().text();
                return;
            }

            QSqlQuery createNew;
            if (!createNew.exec(
                    "CREATE TABLE products_new ("
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
                    ");")) {
                qDebug() << "CREATE TABLE products_new ERROR:" << createNew.lastError().text();
                query.exec("ROLLBACK");
                return;
            }

            QSqlQuery selectOld;
            if (!selectOld.exec(
                    "SELECT id, name, sku, category, price, quantity, unit, location, description "
                    "FROM products")) {
                qDebug() << "SELECT old products ERROR:" << selectOld.lastError().text();
                query.exec("ROLLBACK");
                return;
            }

            QSqlQuery insertCat;
            insertCat.prepare("INSERT OR IGNORE INTO categories (name) VALUES (:name)");

            QSqlQuery selectCatId;
            selectCatId.prepare("SELECT id FROM categories WHERE name = :name");

            QSqlQuery insertNew;
            insertNew.prepare(
                "INSERT INTO products_new (id, name, sku, category_id, price, quantity, unit, location, description) "
                "VALUES (:id, :name, :sku, :category_id, :price, :quantity, :unit, :location, :description)"
            );

            while (selectOld.next()) {
                const int id = selectOld.value("id").toInt();
                const QString name = selectOld.value("name").toString();
                const QString sku = selectOld.value("sku").toString();
                QString categoryName = selectOld.value("category").toString().trimmed();
                if (categoryName.isEmpty())
                    categoryName = QStringLiteral("Ogólna");

                insertCat.bindValue(":name", categoryName);
                if (!insertCat.exec()) {
                    qDebug() << "INSERT category during migration ERROR:" << insertCat.lastError().text();
                    query.exec("ROLLBACK");
                    return;
                }

                selectCatId.bindValue(":name", categoryName);
                if (!selectCatId.exec() || !selectCatId.next()) {
                    qDebug() << "SELECT category id during migration ERROR:" << selectCatId.lastError().text();
                    query.exec("ROLLBACK");
                    return;
                }
                const int categoryId = selectCatId.value(0).toInt();

                insertNew.bindValue(":id", id);
                insertNew.bindValue(":name", name);
                insertNew.bindValue(":sku", sku);
                insertNew.bindValue(":category_id", categoryId);
                insertNew.bindValue(":price", selectOld.value("price").toDouble());
                insertNew.bindValue(":quantity", selectOld.value("quantity").toDouble());
                insertNew.bindValue(":unit", selectOld.value("unit").toString());
                insertNew.bindValue(":location", selectOld.value("location").toString());
                insertNew.bindValue(":description", selectOld.value("description").toString());

                if (!insertNew.exec()) {
                    qDebug() << "INSERT migrated product ERROR:" << insertNew.lastError().text();
                    query.exec("ROLLBACK");
                    return;
                }
            }

            if (!query.exec("DROP TABLE products")) {
                qDebug() << "DROP old products ERROR:" << query.lastError().text();
                query.exec("ROLLBACK");
                return;
            }

            if (!query.exec("ALTER TABLE products_new RENAME TO products")) {
                qDebug() << "RENAME products_new ERROR:" << query.lastError().text();
                query.exec("ROLLBACK");
                return;
            }

            if (!query.exec("COMMIT")) {
                qDebug() << "COMMIT migration ERROR:" << query.lastError().text();
                query.exec("ROLLBACK");
                return;
            }
        }
    }

    // Ensure indexes on products
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_products_sku ON products(sku);")) {
        qDebug() << "CREATE INDEX products ERROR:" << query.lastError().text();
    }
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_products_category_id ON products(category_id);")) {
        qDebug() << "CREATE INDEX products ERROR:" << query.lastError().text();
    }
}
