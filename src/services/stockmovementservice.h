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

    // Posts a movement (creates header + lines). For Issue/Adjust it validates that stock never goes below 0.
    // For Relocate it updates products.location to header.toLocation.
    static bool postMovement(const StockMovement& header, const QList<StockMovementLine>& lines, int* outMovementId = nullptr);

    // Cancels movement (does not delete). Canceled movements should be ignored in stock calculations.
    static bool cancelMovement(int movementId);

    // Current stock based on history (non-canceled, affects_stock=1).
    static double getCurrentStockForProduct(int productId);

private:
    static bool validateIssueLikeMovementDoesNotGoNegative(MovementType type, const QList<StockMovementLine>& normalizedLines, QString* outError);
    static QList<StockMovementLine> normalizeLinesForType(MovementType type, const QList<StockMovementLine>& lines);
};
