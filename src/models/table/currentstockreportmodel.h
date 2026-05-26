#pragma once

#include <QAbstractTableModel>

#include "../product.h"

class CurrentStockReportModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        Sku = 0,
        Name,
        Quantity,
        Unit,
        Location,
        ColumnCount
    };

    explicit CurrentStockReportModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void reload();

    QList<Product> products() const;

private:
    QList<Product> m_products;
};
