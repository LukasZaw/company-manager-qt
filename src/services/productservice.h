#pragma once

#include <QList>

#include "../models/product.h"

class ProductService {
public:
    static QList<Product> getAllProducts();
    static Product getProductById(int id);
    static bool addProduct(const Product& product);
    static bool updateProduct(const Product& product);
    static bool deleteProduct(int id);
};
