#pragma once

#include <QString>

class Location {
public:
    int id{0};
    int parentId{0}; // 0 means NULL/root
    QString name;
    QString path; // materialized path: Magazyn/Sektor A/Regał 5/Półka 2
};
