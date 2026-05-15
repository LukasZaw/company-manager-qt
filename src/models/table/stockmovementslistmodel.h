#pragma once

#include <QAbstractListModel>
#include <QList>

#include "../stockmovement.h"

class StockMovementsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TypeDisplayRole,
        OccurredAtRole,
        OccurredAtDisplayRole,
        EmployeeIdRole,
        EmployeeNameRole,
        FromLocationRole,
        ToLocationRole,
        NotesRole,
        CanceledRole,
        AffectsStockRole,
        LinesCountRole,
    };

    explicit StockMovementsListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setMovements(const QList<StockMovement>& movements);
    StockMovement getMovement(int row) const;

private:
    QList<StockMovement> m_movements;
};
