#pragma once

#include <QList>
#include <QString>

#include "../models/stockmovement.h"

class StockMovementService {
public:
    static QList<StockMovement> getMovements(const QString& search = QString());
    static QList<StockMovement> getMovementsByType(MovementType type, const QString& search = QString());

    static StockMovement getMovementById(int id);
    static QList<StockMovementLine> getMovementLines(int movementId);

    static bool postMovement(const StockMovement& header, const QList<StockMovementLine>& lines, int* outMovementId = nullptr);

    static bool cancelMovement(int movementId);

    static double getCurrentStockForProduct(int productId);

private:
    static bool validateIssueLikeMovementDoesNotGoNegative(MovementType type, const QList<StockMovementLine>& normalizedLines, QString* outError);
    static QList<StockMovementLine> normalizeLinesForType(MovementType type, const QList<StockMovementLine>& lines);
};
