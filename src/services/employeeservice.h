#pragma once

#include <QList>
#include "../models/employee.h"

class EmployeeService {
public:
    static QList<Employee> getAllEmployees();
    static Employee getEmployeeById(int id);
    static bool addEmployee(const Employee& employee);
    static bool updateEmployee(const Employee& employee);
    static bool deleteEmployee(int id);
};
