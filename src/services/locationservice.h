#pragma once

#include <QList>
#include <QString>

#include "../models/location.h"

class LocationService {
public:
    static QList<Location> getAllLocations();
    static Location getLocationById(int id);

    static bool addLocation(int parentId, const QString& name, int* outId = nullptr);
    static bool renameLocation(int id, const QString& newName);
    static bool deleteLocation(int id);

    static QString getPathById(int id);

private:
    static QString sanitizeSegment(const QString& name);
};
