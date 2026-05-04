#pragma once

#include <QList>
#include "../models/employee.h"

class EmployeeService {
public:
    static QList<Employee> getAllEmployees();
    static bool addEmployee(const Employee& employee);
    static bool deleteEmployee(int id);
};
