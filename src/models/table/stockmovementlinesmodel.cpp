#include "stockmovementlinesmodel.h"

#include <QFontMetrics>

StockMovementLinesModel::StockMovementLinesModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int StockMovementLinesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_lines.size();
}

int StockMovementLinesModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant StockMovementLinesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const int row = index.row();
    if (row < 0 || row >= m_lines.size())
        return {};

    const StockMovementLine& l = m_lines[row];

    const ProductCatalogItem catalogItem = m_catalog.contains(l.productId) ? m_catalog.value(l.productId) : ProductCatalogItem{};
    const QString effectiveSku = !l.sku.trimmed().isEmpty() ? l.sku.trimmed() : catalogItem.sku.trimmed();
    const QString effectiveName = !l.productName.trimmed().isEmpty() ? l.productName.trimmed() : catalogItem.name.trimmed();
    const QString effectiveUnit = !l.unit.trimmed().isEmpty() ? l.unit.trimmed() : catalogItem.unit.trimmed();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Product: {
            if (!effectiveSku.isEmpty() && !effectiveName.isEmpty())
                return QString("%1 — %2").arg(effectiveSku, effectiveName);
            if (!effectiveSku.isEmpty())
                return effectiveSku;
            return effectiveName;
        }
        case Quantity:
            return l.quantity;
        case Unit:
            return effectiveUnit;
        default:
            return {};
        }
    }

    if (role == Qt::EditRole) {
        switch (index.column()) {
        case Product:
            return l.productId;
        case Quantity:
            return l.quantity;
        case Unit:
            return effectiveUnit;
        default:
            return {};
        }
    }

    switch (role) {
    case LineIdRole:
        return l.id;
    case MovementIdRole:
        return l.movementId;
    case ProductIdRole:
        return l.productId;
    case SkuRole:
        return l.sku;
    case ProductNameRole:
        return l.productName;
    case UnitRole:
        return l.unit;
    case QuantityRole:
        return l.quantity;
    default:
        return {};
    }
}

QVariant StockMovementLinesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case Product:
        return "Produkt";
    case Quantity:
        return "Ilość";
    case Unit:
        return "Jednostka";
    default:
        return {};
    }
}

Qt::ItemFlags StockMovementLinesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Allow editing in the UI. Product selection is typically done via delegate (combobox/completer).
    if (index.column() == Product || index.column() == Quantity) {
        f |= Qt::ItemIsEditable;
    }

    return f;
}

bool StockMovementLinesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    const int row = index.row();
    if (row < 0 || row >= m_lines.size())
        return false;

    StockMovementLine& l = m_lines[row];

    if (role == Qt::EditRole) {
        if (index.column() == Quantity) {
            bool ok = false;
            const double q = value.toDouble(&ok);
            if (!ok)
                return false;
            if (q == l.quantity)
                return false;

            l.quantity = q;
            emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole, QuantityRole });
            return true;
        }

        if (index.column() == Product) {
            // By default treat EditRole as productId. Caller can also use setLine/setLines.
            bool ok = false;
            const int productId = value.toInt(&ok);
            if (!ok)
                return false;
            if (productId == l.productId)
                return false;

            l.productId = productId;

            if (m_catalog.contains(productId)) {
                const auto ci = m_catalog.value(productId);
                l.sku = ci.sku;
                l.productName = ci.name;
                l.unit = ci.unit;
            }

            emit dataChanged(this->index(row, 0), this->index(row, ColumnCount - 1), { Qt::DisplayRole, Qt::EditRole, ProductIdRole, SkuRole, ProductNameRole, UnitRole });
            return true;
        }

        return false;
    }

    if (role == ProductIdRole) {
        const int productId = value.toInt();
        if (productId == l.productId)
            return false;
        l.productId = productId;

        if (m_catalog.contains(productId)) {
            const auto ci = m_catalog.value(productId);
            l.sku = ci.sku;
            l.productName = ci.name;
            l.unit = ci.unit;
        }

        emit dataChanged(this->index(row, 0), this->index(row, ColumnCount - 1), { Qt::DisplayRole, Qt::EditRole, ProductIdRole, SkuRole, ProductNameRole, UnitRole });
        return true;
    }

    if (role == QuantityRole) {
        const double q = value.toDouble();
        if (q == l.quantity)
            return false;
        l.quantity = q;
        emit dataChanged(this->index(row, Quantity), this->index(row, Quantity), { Qt::DisplayRole, Qt::EditRole, QuantityRole });
        return true;
    }

    return false;
}

bool StockMovementLinesModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid())
        return false;
    if (count <= 0)
        return false;

    const int insertRow = qBound(0, row, m_lines.size());

    beginInsertRows(QModelIndex(), insertRow, insertRow + count - 1);
    for (int i = 0; i < count; ++i) {
        m_lines.insert(insertRow, StockMovementLine{});
    }
    endInsertRows();

    return true;
}

bool StockMovementLinesModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid())
        return false;
    if (count <= 0)
        return false;
    if (row < 0 || row >= m_lines.size())
        return false;

    const int last = qMin(row + count - 1, m_lines.size() - 1);

    beginRemoveRows(QModelIndex(), row, last);
    for (int r = last; r >= row; --r) {
        m_lines.removeAt(r);
    }
    endRemoveRows();

    return true;
}

void StockMovementLinesModel::setLines(const QList<StockMovementLine>& lines)
{
    beginResetModel();
    m_lines = lines;
    endResetModel();
}

void StockMovementLinesModel::setProductCatalog(const QHash<int, ProductCatalogItem>& catalog)
{
    m_catalog = catalog;
    if (!m_lines.isEmpty()) {
        emit dataChanged(index(0, 0), index(m_lines.size() - 1, ColumnCount - 1), { Qt::DisplayRole, Qt::EditRole });
    }
}

QList<StockMovementLine> StockMovementLinesModel::lines() const
{
    return m_lines;
}

StockMovementLine StockMovementLinesModel::getLine(int row) const
{
    if (row < 0 || row >= m_lines.size())
        return StockMovementLine{};

    return m_lines.at(row);
}

void StockMovementLinesModel::setLine(int row, const StockMovementLine& line)
{
    if (row < 0 || row >= m_lines.size())
        return;

    m_lines[row] = line;
    emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
}
