#pragma once

#include <QAbstractTableModel>
#include <QList>

#include "../product.h"

class ProductTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ProductTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setProducts(const QList<Product>& products);

    Product getProduct(int row) const;

private:
    QList<Product> m_products;
};
