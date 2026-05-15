#include "stockmovementslistmodel.h"

#include <QDateTime>

static QString occurredAtToDisplay(const QDateTime& dt)
{
    if (!dt.isValid())
        return QString();
    return dt.toString("yyyy-MM-dd HH:mm");
}

StockMovementsListModel::StockMovementsListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int StockMovementsListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_movements.size();
}

QVariant StockMovementsListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const int row = index.row();
    if (row < 0 || row >= m_movements.size())
        return {};

    const StockMovement& m = m_movements[row];

    switch (role) {
    case Qt::DisplayRole: {
        const QString typeName = movementTypeDisplayName(m.type);
        const QString dateStr = occurredAtToDisplay(m.occurredAt);
        if (dateStr.isEmpty())
            return typeName;
        return QString("%1 • %2").arg(typeName, dateStr);
    }
    case Qt::ToolTipRole:
        return m.notes;

    case IdRole:
        return m.id;
    case TypeRole:
        return movementTypeToDbString(m.type);
    case TypeDisplayRole:
        return movementTypeDisplayName(m.type);
    case OccurredAtRole:
        return m.occurredAt;
    case OccurredAtDisplayRole:
        return occurredAtToDisplay(m.occurredAt);
    case EmployeeIdRole:
        return m.employeeId;
    case EmployeeNameRole:
        return m.employeeName;
    case FromLocationRole:
        return m.fromLocation;
    case ToLocationRole:
        return m.toLocation;
    case NotesRole:
        return m.notes;
    case CanceledRole:
        return m.canceled;
    case AffectsStockRole:
        return m.affectsStock;
    case LinesCountRole:
        return m.linesCount;

    default:
        return {};
    }
}

QHash<int, QByteArray> StockMovementsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TypeRole] = "type";
    roles[TypeDisplayRole] = "typeDisplay";
    roles[OccurredAtRole] = "occurredAt";
    roles[OccurredAtDisplayRole] = "occurredAtDisplay";
    roles[EmployeeIdRole] = "employeeId";
    roles[EmployeeNameRole] = "employeeName";
    roles[FromLocationRole] = "fromLocation";
    roles[ToLocationRole] = "toLocation";
    roles[NotesRole] = "notes";
    roles[CanceledRole] = "canceled";
    roles[AffectsStockRole] = "affectsStock";
    roles[LinesCountRole] = "linesCount";
    return roles;
}

void StockMovementsListModel::setMovements(const QList<StockMovement>& movements)
{
    beginResetModel();
    m_movements = movements;
    endResetModel();
}

StockMovement StockMovementsListModel::getMovement(int row) const
{
    if (row < 0 || row >= m_movements.size())
        return StockMovement{};
    return m_movements.at(row);
}
