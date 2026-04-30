#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

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

#endif // DATABASEMANAGER_H
