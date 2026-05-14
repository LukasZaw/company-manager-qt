#pragma once

#include <QString>

class Product {
public:
    int id{0};
    QString name;
    QString sku;
    int categoryId{0};
    QString category; // display name (from categories table)
    double price{0.0};
    double quantity{0.0};
    QString unit;
    QString location;
    QString description;
};
