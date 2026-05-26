#pragma once

#include <QString>
#include <QDate>

class Employee {
public:
    int id{0};
    QString firstName;
    QString lastName;
    QString email;
    QString phone;
    QString position;
    int departmentId{0};
    QString department; // display name
    QDate hireDate;
    bool active{true};
    QString notes;
};
