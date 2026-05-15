#pragma once

#include <QAbstractTableModel>
#include <QHash>
#include <QList>

#include "../stockmovement.h"

class StockMovementLinesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct ProductCatalogItem {
        QString sku;
        QString name;
        QString unit;
    };

    enum Column {
        Product = 0,
        Quantity = 1,
        Unit = 2,
        ColumnCount = 3,
    };

    enum Roles {
        LineIdRole = Qt::UserRole + 1,
        MovementIdRole,
        ProductIdRole,
        SkuRole,
        ProductNameRole,
        UnitRole,
        QuantityRole,
    };

    explicit StockMovementLinesModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

    void setLines(const QList<StockMovementLine>& lines);
    QList<StockMovementLine> lines() const;

    void setProductCatalog(const QHash<int, ProductCatalogItem>& catalog);

    StockMovementLine getLine(int row) const;
    void setLine(int row, const StockMovementLine& line);

private:
    QList<StockMovementLine> m_lines;
    QHash<int, ProductCatalogItem> m_catalog;
};
