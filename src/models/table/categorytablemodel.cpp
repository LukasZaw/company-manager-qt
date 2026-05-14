#include "categorytablemodel.h"

CategoryTableModel::CategoryTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int CategoryTableModel::rowCount(const QModelIndex&) const {
    return m_categories.size();
}

int CategoryTableModel::columnCount(const QModelIndex&) const {
    return 2; // ID, Nazwa
}

QVariant CategoryTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    if (index.row() < 0 || index.row() >= m_categories.size())
        return {};

    const auto& category = m_categories.at(index.row());

    switch (index.column()) {
    case 0:
        return category.id;
    case 1:
        return category.name;
    default:
        return {};
    }
}

QVariant CategoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case 0:
        return QObject::tr("ID");
    case 1:
        return QObject::tr("Nazwa");
    default:
        return {};
    }
}

void CategoryTableModel::setCategories(const QList<Category>& categories) {
    beginResetModel();
    m_categories = categories;
    endResetModel();
}

Category CategoryTableModel::getCategory(int row) const {
    if (row < 0 || row >= m_categories.size())
        return Category{};

    return m_categories.at(row);
}
