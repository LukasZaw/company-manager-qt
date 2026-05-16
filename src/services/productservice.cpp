#include "productservice.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

QList<Product> ProductService::getAllProducts()
{
    QList<Product> list;

    QSqlQuery query;
    if (!query.exec(
            "SELECT p.id, p.name, p.sku, p.category_id, c.name AS category_name, p.price, "
            "       COALESCE(SUM(CASE WHEN (m.is_canceled = 0 AND m.affects_stock = 1) THEN l.quantity ELSE 0 END), 0) AS stock_qty, "
            "       p.unit, p.location_id, COALESCE(loc.path, '') AS location_path, p.description "
            "FROM products p "
            "LEFT JOIN categories c ON c.id = p.category_id "
            "LEFT JOIN locations loc ON loc.id = p.location_id "
            "LEFT JOIN warehouse_movement_lines l ON l.product_id = p.id "
            "LEFT JOIN warehouse_movements m ON m.id = l.movement_id "
            "GROUP BY p.id "
            "ORDER BY p.name")) {
        qDebug() << "SELECT products ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        Product p;
        p.id = query.value("id").toInt();
        p.name = query.value("name").toString();
        p.sku = query.value("sku").toString();
        p.categoryId = query.value("category_id").toInt();
        p.category = query.value("category_name").toString();
        p.price = query.value("price").toDouble();
        p.quantity = query.value("stock_qty").toDouble();
        p.unit = query.value("unit").toString();
        p.locationId = query.value("location_id").toInt();
        p.location = query.value("location_path").toString();
        p.description = query.value("description").toString();
        list.append(p);
    }

    return list;
}

Product ProductService::getProductById(int id)
{
    Product p;
    if (id <= 0)
        return p;

    QSqlQuery query;
    query.prepare(
        "SELECT p.id, p.name, p.sku, p.category_id, c.name AS category_name, p.price, "
        "       COALESCE(SUM(CASE WHEN (m.is_canceled = 0 AND m.affects_stock = 1) THEN l.quantity ELSE 0 END), 0) AS stock_qty, "
        "       p.unit, p.location_id, COALESCE(loc.path, '') AS location_path, p.description "
        "FROM products p "
        "LEFT JOIN categories c ON c.id = p.category_id "
        "LEFT JOIN locations loc ON loc.id = p.location_id "
        "LEFT JOIN warehouse_movement_lines l ON l.product_id = p.id "
        "LEFT JOIN warehouse_movements m ON m.id = l.movement_id "
        "WHERE p.id = :id "
        "GROUP BY p.id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SELECT product by id ERROR:" << query.lastError().text();
        return p;
    }

    if (query.next()) {
        p.id = query.value("id").toInt();
        p.name = query.value("name").toString();
        p.sku = query.value("sku").toString();
        p.categoryId = query.value("category_id").toInt();
        p.category = query.value("category_name").toString();
        p.price = query.value("price").toDouble();
        p.quantity = query.value("stock_qty").toDouble();
        p.unit = query.value("unit").toString();
        p.locationId = query.value("location_id").toInt();
        p.location = query.value("location_path").toString();
        p.description = query.value("description").toString();
    }

    return p;
}

bool ProductService::addProduct(const Product& product)
{
    const QString name = product.name.trimmed();
    const QString sku = product.sku.trimmed();
    if (name.isEmpty() || sku.isEmpty() || product.categoryId <= 0)
        return false;

    QSqlQuery query;
    query.prepare(
        "INSERT INTO products (name, sku, category_id, price, quantity, unit, location_id, description) "
        "VALUES (:name, :sku, :category_id, :price, 0, :unit, :location_id, :description)");

    query.bindValue(":name", name);
    query.bindValue(":sku", sku);
    query.bindValue(":category_id", product.categoryId);
    query.bindValue(":price", product.price);
    query.bindValue(":unit", product.unit.trimmed());
    if (product.locationId > 0)
        query.bindValue(":location_id", product.locationId);
    else
        query.bindValue(":location_id", QVariant(QVariant::Int));
    query.bindValue(":description", product.description.trimmed());

    if (!query.exec()) {
        qDebug() << "INSERT product ERROR:" << query.lastError().text();
        return false;
    }

    return true;
}

bool ProductService::updateProduct(const Product& product)
{
    const QString name = product.name.trimmed();
    const QString sku = product.sku.trimmed();
    if (product.id <= 0 || name.isEmpty() || sku.isEmpty() || product.categoryId <= 0)
        return false;

    QSqlQuery query;
    query.prepare(
        "UPDATE products "
        "SET name = :name, sku = :sku, category_id = :category_id, price = :price, "
        "unit = :unit, location_id = :location_id, description = :description "
        "WHERE id = :id");

    query.bindValue(":name", name);
    query.bindValue(":sku", sku);
    query.bindValue(":category_id", product.categoryId);
    query.bindValue(":price", product.price);
    query.bindValue(":unit", product.unit.trimmed());
    if (product.locationId > 0)
        query.bindValue(":location_id", product.locationId);
    else
        query.bindValue(":location_id", QVariant(QVariant::Int));
    query.bindValue(":description", product.description.trimmed());
    query.bindValue(":id", product.id);

    if (!query.exec()) {
        qDebug() << "UPDATE product ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool ProductService::deleteProduct(int id)
{
    if (id <= 0)
        return false;

    QSqlQuery query;
    query.prepare("DELETE FROM products WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "DELETE product ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
