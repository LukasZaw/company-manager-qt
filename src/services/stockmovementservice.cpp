#include "stockmovementservice.h"

#include <QDebug>
#include <QMap>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtGlobal>

static QString likePatternFor(const QString& s)
{
    const QString trimmed = s.trimmed();
    if (trimmed.isEmpty())
        return QString();
    return "%" + trimmed + "%";
}

QList<StockMovement> StockMovementService::getMovements(const QString& search)
{
    QList<StockMovement> list;

    const QString like = likePatternFor(search);

    QSqlQuery query;
    QString sql =
        "SELECT m.id, m.type, m.occurred_at, m.employee_id, "
        "       COALESCE(e.first_name, '') AS emp_first, COALESCE(e.last_name, '') AS emp_last, "
        "       m.from_location, m.to_location, m.notes, m.is_canceled, m.affects_stock, "
        "       (SELECT COUNT(1) FROM warehouse_movement_lines l WHERE l.movement_id = m.id) AS lines_count "
        "FROM warehouse_movements m "
        "LEFT JOIN employees e ON e.id = m.employee_id ";

    if (!like.isEmpty()) {
        sql +=
            "WHERE (m.notes LIKE :like "
            "   OR m.type LIKE :like "
            "   OR m.from_location LIKE :like "
            "   OR m.to_location LIKE :like "
            "   OR (e.first_name || ' ' || e.last_name) LIKE :like "
            "   OR EXISTS (SELECT 1 "
            "              FROM warehouse_movement_lines l "
            "              JOIN products p ON p.id = l.product_id "
            "              WHERE l.movement_id = m.id AND (p.sku LIKE :like OR p.name LIKE :like))"
            ") ";
    }

    sql += "ORDER BY m.occurred_at DESC, m.id DESC";

    query.prepare(sql);
    if (!like.isEmpty())
        query.bindValue(":like", like);

    if (!query.exec()) {
        qDebug() << "SELECT warehouse_movements ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        StockMovement m;
        m.id = query.value("id").toInt();
        m.type = movementTypeFromDbString(query.value("type").toString());
        m.occurredAt = QDateTime::fromString(query.value("occurred_at").toString(), Qt::ISODate);
        m.employeeId = query.value("employee_id").toInt();

        const QString first = query.value("emp_first").toString().trimmed();
        const QString last = query.value("emp_last").toString().trimmed();
        const QString full = (first + " " + last).trimmed();
        m.employeeName = full;

        m.fromLocation = query.value("from_location").toString();
        m.toLocation = query.value("to_location").toString();
        m.notes = query.value("notes").toString();
        m.canceled = query.value("is_canceled").toInt() != 0;
        m.affectsStock = query.value("affects_stock").toInt() != 0;
        m.linesCount = query.value("lines_count").toInt();

        list.append(m);
    }

    return list;
}

QList<StockMovement> StockMovementService::getMovementsByType(MovementType type, const QString& search)
{
    QList<StockMovement> list;

    const QString like = likePatternFor(search);

    QSqlQuery query;
    QString sql =
        "SELECT m.id, m.type, m.occurred_at, m.employee_id, "
        "       COALESCE(e.first_name, '') AS emp_first, COALESCE(e.last_name, '') AS emp_last, "
        "       m.from_location, m.to_location, m.notes, m.is_canceled, m.affects_stock, "
        "       (SELECT COUNT(1) FROM warehouse_movement_lines l WHERE l.movement_id = m.id) AS lines_count "
        "FROM warehouse_movements m "
        "LEFT JOIN employees e ON e.id = m.employee_id "
        "WHERE m.type = :type ";

    if (!like.isEmpty()) {
        sql +=
            "AND (m.notes LIKE :like "
            "  OR m.from_location LIKE :like "
            "  OR m.to_location LIKE :like "
            "  OR (e.first_name || ' ' || e.last_name) LIKE :like "
            "  OR EXISTS (SELECT 1 "
            "             FROM warehouse_movement_lines l "
            "             JOIN products p ON p.id = l.product_id "
            "             WHERE l.movement_id = m.id AND (p.sku LIKE :like OR p.name LIKE :like))"
            ") ";
    }

    sql += "ORDER BY m.occurred_at DESC, m.id DESC";

    query.prepare(sql);
    query.bindValue(":type", movementTypeToDbString(type));
    if (!like.isEmpty())
        query.bindValue(":like", like);

    if (!query.exec()) {
        qDebug() << "SELECT warehouse_movements by type ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        StockMovement m;
        m.id = query.value("id").toInt();
        m.type = movementTypeFromDbString(query.value("type").toString());
        m.occurredAt = QDateTime::fromString(query.value("occurred_at").toString(), Qt::ISODate);
        m.employeeId = query.value("employee_id").toInt();

        const QString first = query.value("emp_first").toString().trimmed();
        const QString last = query.value("emp_last").toString().trimmed();
        const QString full = (first + " " + last).trimmed();
        m.employeeName = full;

        m.fromLocation = query.value("from_location").toString();
        m.toLocation = query.value("to_location").toString();
        m.notes = query.value("notes").toString();
        m.canceled = query.value("is_canceled").toInt() != 0;
        m.affectsStock = query.value("affects_stock").toInt() != 0;
        m.linesCount = query.value("lines_count").toInt();

        list.append(m);
    }

    return list;
}

StockMovement StockMovementService::getMovementById(int id)
{
    StockMovement m;
    if (id <= 0)
        return m;

    QSqlQuery query;
    query.prepare(
        "SELECT m.id, m.type, m.occurred_at, m.employee_id, "
        "       COALESCE(e.first_name, '') AS emp_first, COALESCE(e.last_name, '') AS emp_last, "
        "       m.from_location, m.to_location, m.notes, m.is_canceled, m.affects_stock, "
        "       (SELECT COUNT(1) FROM warehouse_movement_lines l WHERE l.movement_id = m.id) AS lines_count "
        "FROM warehouse_movements m "
        "LEFT JOIN employees e ON e.id = m.employee_id "
        "WHERE m.id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "SELECT warehouse_movement by id ERROR:" << query.lastError().text();
        return m;
    }

    if (!query.next())
        return m;

    m.id = query.value("id").toInt();
    m.type = movementTypeFromDbString(query.value("type").toString());
    m.occurredAt = QDateTime::fromString(query.value("occurred_at").toString(), Qt::ISODate);
    m.employeeId = query.value("employee_id").toInt();

    const QString first = query.value("emp_first").toString().trimmed();
    const QString last = query.value("emp_last").toString().trimmed();
    m.employeeName = (first + " " + last).trimmed();

    m.fromLocation = query.value("from_location").toString();
    m.toLocation = query.value("to_location").toString();
    m.notes = query.value("notes").toString();
    m.canceled = query.value("is_canceled").toInt() != 0;
    m.affectsStock = query.value("affects_stock").toInt() != 0;
    m.linesCount = query.value("lines_count").toInt();

    return m;
}

QList<StockMovementLine> StockMovementService::getMovementLines(int movementId)
{
    QList<StockMovementLine> list;
    if (movementId <= 0)
        return list;

    QSqlQuery query;
    query.prepare(
        "SELECT l.id, l.movement_id, l.product_id, l.quantity, "
        "       p.sku, p.name AS product_name, p.unit "
        "FROM warehouse_movement_lines l "
        "JOIN products p ON p.id = l.product_id "
        "WHERE l.movement_id = :movement_id "
        "ORDER BY l.id");
    query.bindValue(":movement_id", movementId);

    if (!query.exec()) {
        qDebug() << "SELECT warehouse_movement_lines ERROR:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        StockMovementLine l;
        l.id = query.value("id").toInt();
        l.movementId = query.value("movement_id").toInt();
        l.productId = query.value("product_id").toInt();
        l.quantity = query.value("quantity").toDouble();
        l.sku = query.value("sku").toString();
        l.productName = query.value("product_name").toString();
        l.unit = query.value("unit").toString();
        list.append(l);
    }

    return list;
}

QList<StockMovementLine> StockMovementService::normalizeLinesForType(MovementType type, const QList<StockMovementLine>& lines)
{
    QList<StockMovementLine> normalized;
    normalized.reserve(lines.size());

    for (const auto& l : lines) {
        StockMovementLine nl = l;
        const double q = nl.quantity;

        if (q == 0.0)
            continue;

        switch (type) {
        case MovementType::Receipt:
            nl.quantity = qAbs(q);
            break;
        case MovementType::Issue:
            nl.quantity = -qAbs(q);
            break;
        case MovementType::Relocate:
            nl.quantity = qAbs(q);
            break;
        case MovementType::Adjust:
            // keep sign
            break;
        default:
            break;
        }

        normalized.append(nl);
    }

    return normalized;
}

bool StockMovementService::validateIssueLikeMovementDoesNotGoNegative(
    MovementType type,
    const QList<StockMovementLine>& normalizedLines,
    QString* outError)
{
    if (type != MovementType::Issue && type != MovementType::Adjust)
        return true;

    // Aggregate delta per product_id for this movement
    QMap<int, double> deltaByProduct;
    for (const auto& l : normalizedLines) {
        if (l.productId <= 0)
            continue;
        deltaByProduct[l.productId] += l.quantity;
    }

    // For Issue: all deltas should be negative.
    // For Adjust: deltas can be positive or negative; only validate ones that reduce stock.
    for (auto it = deltaByProduct.constBegin(); it != deltaByProduct.constEnd(); ++it) {
        const int productId = it.key();
        const double delta = it.value();

        if (type == MovementType::Issue && delta >= 0)
            continue;
        if (type == MovementType::Adjust && delta >= 0)
            continue;

        const double current = getCurrentStockForProduct(productId);
        const double after = current + delta;

        if (after < -1e-9) {
            if (outError) {
                *outError = QString("Brak wystarczającego stanu dla produktu ID %1. Stan: %2, zmiana: %3")
                                .arg(productId)
                                .arg(current)
                                .arg(delta);
            }
            return false;
        }
    }

    return true;
}

bool StockMovementService::postMovement(const StockMovement& header, const QList<StockMovementLine>& lines, int* outMovementId)
{
    if (outMovementId)
        *outMovementId = 0;

    if (lines.isEmpty())
        return false;

    StockMovement h = header;
    h.notes = h.notes.trimmed();
    h.fromLocation = h.fromLocation.trimmed();
    h.toLocation = h.toLocation.trimmed();

    if (!h.occurredAt.isValid())
        h.occurredAt = QDateTime::currentDateTime();

    const QList<StockMovementLine> normalizedLines = normalizeLinesForType(h.type, lines);
    if (normalizedLines.isEmpty())
        return false;

    // Type-specific sanity checks
    if (h.type == MovementType::Relocate) {
        h.affectsStock = false;
        if (h.toLocation.isEmpty())
            return false;
    } else {
        h.affectsStock = true;
    }

    for (const auto& l : normalizedLines) {
        if (l.productId <= 0)
            return false;
        if (l.quantity == 0.0)
            return false;
    }

    QString validationError;
    if (!validateIssueLikeMovementDoesNotGoNegative(h.type, normalizedLines, &validationError)) {
        qDebug() << "postMovement validation failed:" << validationError;
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid() || !db.isOpen())
        return false;

    if (!db.transaction()) {
        qDebug() << "DB transaction ERROR:" << db.lastError().text();
        return false;
    }

    int movementId = 0;

    {
        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO warehouse_movements (type, occurred_at, employee_id, from_location, to_location, notes, is_canceled, affects_stock) "
            "VALUES (:type, :occurred_at, :employee_id, :from_location, :to_location, :notes, 0, :affects_stock)");

        query.bindValue(":type", movementTypeToDbString(h.type));
        query.bindValue(":occurred_at", h.occurredAt.toString(Qt::ISODate));

        if (h.employeeId > 0)
            query.bindValue(":employee_id", h.employeeId);
        else
            query.bindValue(":employee_id", QVariant(QVariant::Int));

        if (h.fromLocation.isEmpty())
            query.bindValue(":from_location", QVariant(QVariant::String));
        else
            query.bindValue(":from_location", h.fromLocation);

        if (h.toLocation.isEmpty())
            query.bindValue(":to_location", QVariant(QVariant::String));
        else
            query.bindValue(":to_location", h.toLocation);

        if (h.notes.isEmpty())
            query.bindValue(":notes", QVariant(QVariant::String));
        else
            query.bindValue(":notes", h.notes);

        query.bindValue(":affects_stock", h.affectsStock ? 1 : 0);

        if (!query.exec()) {
            qDebug() << "INSERT warehouse_movements ERROR:" << query.lastError().text();
            db.rollback();
            return false;
        }

        movementId = query.lastInsertId().toInt();
        if (movementId <= 0) {
            qDebug() << "INSERT warehouse_movements ERROR: invalid lastInsertId";
            db.rollback();
            return false;
        }
    }

    {
        QSqlQuery query(db);
        query.prepare(
            "INSERT INTO warehouse_movement_lines (movement_id, product_id, quantity) "
            "VALUES (:movement_id, :product_id, :quantity)");

        for (const auto& l : normalizedLines) {
            query.bindValue(":movement_id", movementId);
            query.bindValue(":product_id", l.productId);
            query.bindValue(":quantity", l.quantity);

            if (!query.exec()) {
                qDebug() << "INSERT warehouse_movement_lines ERROR:" << query.lastError().text();
                db.rollback();
                return false;
            }
        }
    }

    if (h.type == MovementType::Relocate) {
        QSet<int> uniqueProducts;
        for (const auto& l : normalizedLines)
            uniqueProducts.insert(l.productId);

        QSqlQuery query(db);
        query.prepare("UPDATE products SET location = :location WHERE id = :id");

        for (const int productId : uniqueProducts) {
            query.bindValue(":location", h.toLocation);
            query.bindValue(":id", productId);

            if (!query.exec()) {
                qDebug() << "UPDATE products.location ERROR:" << query.lastError().text();
                db.rollback();
                return false;
            }
        }
    }

    if (!db.commit()) {
        qDebug() << "DB commit ERROR:" << db.lastError().text();
        db.rollback();
        return false;
    }

    if (outMovementId)
        *outMovementId = movementId;

    return true;
}

bool StockMovementService::cancelMovement(int movementId)
{
    if (movementId <= 0)
        return false;

    QSqlQuery query;
    query.prepare("UPDATE warehouse_movements SET is_canceled = 1 WHERE id = :id");
    query.bindValue(":id", movementId);

    if (!query.exec()) {
        qDebug() << "CANCEL warehouse_movement ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

double StockMovementService::getCurrentStockForProduct(int productId)
{
    if (productId <= 0)
        return 0.0;

    QSqlQuery query;
    query.prepare(
        "SELECT COALESCE(SUM(l.quantity), 0) AS qty "
        "FROM warehouse_movement_lines l "
        "JOIN warehouse_movements m ON m.id = l.movement_id "
        "WHERE l.product_id = :product_id "
        "  AND m.is_canceled = 0 "
        "  AND m.affects_stock = 1");
    query.bindValue(":product_id", productId);

    if (!query.exec()) {
        qDebug() << "SELECT current stock ERROR:" << query.lastError().text();
        return 0.0;
    }

    if (!query.next())
        return 0.0;

    return query.value("qty").toDouble();
}
