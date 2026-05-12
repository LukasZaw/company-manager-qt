#pragma once

#include <QList>
#include <QString>
#include "../models/department.h"

class DepartmentService {
public:
    static QList<Department> getAllDepartments();
    static bool addDepartment(const QString& name);
    static bool updateDepartment(int id, const QString& name);
    static bool deleteDepartment(int id, QString* errorMessage = nullptr);
};
