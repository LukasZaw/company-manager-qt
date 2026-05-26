#include "currentstockreportmodel.h"

#include "../../services/productservice.h"

#include <QLocale>

CurrentStockReportModel::CurrentStockReportModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    reload();
}

int CurrentStockReportModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_products.size();
}

int CurrentStockReportModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant CurrentStockReportModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const int row = index.row();
    if (row < 0 || row >= m_products.size())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const Product& p = m_products.at(row);

    switch (index.column()) {
    case Sku:
        return p.sku;
    case Name:
        return p.name;
    case Quantity:
        return QLocale().toString(p.quantity);
    case Unit:
        return p.unit;
    case Location:
        return p.location;
    default:
        return {};
    }
}

QVariant CurrentStockReportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case Sku:
        return tr("SKU");
    case Name:
        return tr("Produkt");
    case Quantity:
        return tr("Stan");
    case Unit:
        return tr("Jedn.");
    case Location:
        return tr("Lokalizacja");
    default:
        return {};
    }
}

void CurrentStockReportModel::reload()
{
    beginResetModel();
    m_products = ProductService::getAllProducts();
    endResetModel();
}

QList<Product> CurrentStockReportModel::products() const
{
    return m_products;
}
