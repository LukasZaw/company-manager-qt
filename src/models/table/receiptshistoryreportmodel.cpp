#include "receiptshistoryreportmodel.h"

#include <QLocale>
#include <QSqlError>
#include <QSqlQuery>

ReceiptsHistoryReportModel::ReceiptsHistoryReportModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    reload();
}

int ReceiptsHistoryReportModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_rows.size();
}

int ReceiptsHistoryReportModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant ReceiptsHistoryReportModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const int row = index.row();
    if (row < 0 || row >= m_rows.size())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const Row& r = m_rows.at(row);

    switch (index.column()) {
    case OccurredAt:
        return r.occurredAt.isValid() ? r.occurredAt.toString("yyyy-MM-dd HH:mm") : QString();
    case MovementId:
        return r.movementId;
    case Employee:
        return r.employee;
    case Sku:
        return r.sku;
    case Product:
        return r.product;
    case Quantity:
        return QLocale().toString(r.quantity);
    case Unit:
        return r.unit;
    case ToLocation:
        return r.toLocation;
    case Notes:
        return r.notes;
    case Canceled:
        return r.canceled ? tr("Tak") : tr("Nie");
    default:
        return {};
    }
}

QVariant ReceiptsHistoryReportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case OccurredAt:
        return tr("Data");
    case MovementId:
        return tr("Dok.");
    case Employee:
        return tr("Pracownik");
    case Sku:
        return tr("SKU");
    case Product:
        return tr("Produkt");
    case Quantity:
        return tr("Ilość");
    case Unit:
        return tr("Jedn.");
    case ToLocation:
        return tr("Lokalizacja");
    case Notes:
        return tr("Uwagi");
    case Canceled:
        return tr("Anulowane");
    default:
        return {};
    }
}

void ReceiptsHistoryReportModel::reload()
{
    beginResetModel();
    m_rows.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT m.id AS movement_id, m.occurred_at, "
        "       COALESCE(e.first_name, '') AS emp_first, COALESCE(e.last_name, '') AS emp_last, "
        "       COALESCE(lt.path, '') AS to_location, COALESCE(m.notes, '') AS notes, "
        "       m.is_canceled, "
        "       l.quantity, p.sku, p.name AS product_name, COALESCE(p.unit, '') AS unit "
        "FROM warehouse_movements m "
        "JOIN warehouse_movement_lines l ON l.movement_id = m.id "
        "JOIN products p ON p.id = l.product_id "
        "LEFT JOIN employees e ON e.id = m.employee_id "
        "LEFT JOIN locations lt ON lt.id = m.to_location_id "
        "WHERE m.type = 'RECEIPT' "
        "ORDER BY m.occurred_at DESC, m.id DESC, l.id ASC");

    if (query.exec()) {
        while (query.next()) {
            Row r;
            r.movementId = query.value("movement_id").toInt();
            r.occurredAt = QDateTime::fromString(query.value("occurred_at").toString(), Qt::ISODate);

            const QString first = query.value("emp_first").toString().trimmed();
            const QString last = query.value("emp_last").toString().trimmed();
            r.employee = (first + " " + last).trimmed();

            r.toLocation = query.value("to_location").toString();
            r.notes = query.value("notes").toString();
            r.canceled = query.value("is_canceled").toInt() != 0;

            r.quantity = query.value("quantity").toDouble();
            r.sku = query.value("sku").toString();
            r.product = query.value("product_name").toString();
            r.unit = query.value("unit").toString();

            m_rows.push_back(r);
        }
    }

    endResetModel();
}

QVector<ReceiptsHistoryReportModel::Row> ReceiptsHistoryReportModel::rows() const
{
    return m_rows;
}
