#include "producttablemodel.h"

ProductTableModel::ProductTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int ProductTableModel::rowCount(const QModelIndex&) const
{
    return m_products.size();
}

int ProductTableModel::columnCount(const QModelIndex&) const
{
    return 9; // ID, Nazwa, SKU, Kategoria, Cena, Stan, Jednostka, Lokalizacja, Opis
}

QVariant ProductTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const Product& product = m_products[index.row()];

    switch (index.column()) {
    case 0:
        return product.id;
    case 1:
        return product.name;
    case 2:
        return product.sku;
    case 3:
        return product.category;
    case 4:
        return product.price;
    case 5:
        return product.quantity;
    case 6:
        return product.unit;
    case 7:
        return product.location;
    case 8:
        return product.description;
    default:
        return {};
    }
}

QVariant ProductTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case 0:
        return "ID";
    case 1:
        return "Nazwa produktu";
    case 2:
        return "SKU";
    case 3:
        return "Kategoria";
    case 4:
        return "Cena";
    case 5:
        return "Stan";
    case 6:
        return "Jednostka";
    case 7:
        return "Lokalizacja";
    case 8:
        return "Opis";
    default:
        return {};
    }
}

void ProductTableModel::setProducts(const QList<Product>& products)
{
    beginResetModel();
    m_products = products;
    endResetModel();
}

Product ProductTableModel::getProduct(int row) const
{
    if (row < 0 || row >= m_products.size())
        return Product{};

    return m_products.at(row);
}
