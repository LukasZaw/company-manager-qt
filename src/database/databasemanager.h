#pragma once

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static bool connect();
    static QSqlDatabase getDatabase();

private:
    static QSqlDatabase db;
    static void initialize();
};
