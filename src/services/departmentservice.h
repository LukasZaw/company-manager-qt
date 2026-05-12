#pragma once

#include <QList>
#include "../models/department.h"

class DepartmentService {
public:
    static QList<Department> getAllDepartments();
};
