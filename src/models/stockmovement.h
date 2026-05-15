#pragma once

#include <QDateTime>
#include <QString>

enum class MovementType {
    Receipt,
    Issue,
    Relocate,
    Adjust,
};

inline QString movementTypeToDbString(MovementType type)
{
    switch (type) {
    case MovementType::Receipt:
        return "RECEIPT";
    case MovementType::Issue:
        return "ISSUE";
    case MovementType::Relocate:
        return "RELOCATE";
    case MovementType::Adjust:
        return "ADJUST";
    default:
        return "RECEIPT";
    }
}

inline MovementType movementTypeFromDbString(const QString& s)
{
    const QString upper = s.trimmed().toUpper();
    if (upper == "RECEIPT")
        return MovementType::Receipt;
    if (upper == "ISSUE")
        return MovementType::Issue;
    if (upper == "RELOCATE")
        return MovementType::Relocate;
    if (upper == "ADJUST")
        return MovementType::Adjust;

    return MovementType::Receipt;
}

inline QString movementTypeDisplayName(MovementType type)
{
    switch (type) {
    case MovementType::Receipt:
        return "Przyjęcie";
    case MovementType::Issue:
        return "Wydanie";
    case MovementType::Relocate:
        return "Przesunięcie";
    case MovementType::Adjust:
        return "Korekta";
    default:
        return "Przyjęcie";
    }
}

class StockMovement {
public:
    int id{0};
    MovementType type{MovementType::Receipt};
    QDateTime occurredAt;

    int employeeId{0};
    QString employeeName; // display name

    QString fromLocation;
    QString toLocation;

    QString notes;

    bool canceled{false};
    bool affectsStock{true};

    int linesCount{0};
};

class StockMovementLine {
public:
    int id{0};
    int movementId{0};

    int productId{0};
    QString sku;
    QString productName;
    QString unit;

    double quantity{0.0}; // signed receipt +, issue -
};
