#pragma once

#include <QList>
#include <QString>

#include "../models/category.h"

class CategoryService {
public:
    static QList<Category> getAllCategories();
    static bool addCategory(const QString& name);
    static bool updateCategory(int id, const QString& name);
    static bool deleteCategory(int id, QString* errorMessage = nullptr);
};
