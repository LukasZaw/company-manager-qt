#pragma once

#include <QString>

class Location {
public:
    int id{0};
    int parentId{0}; // 0 means root
    QString name;
    QString path;
};
