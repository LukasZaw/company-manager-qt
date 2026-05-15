#pragma once

#include <QHash>
#include <QStyledItemDelegate>

class ProductComboDelegate : public QStyledItemDelegate
{
public:
    struct ProductItem {
        int id{0};
        QString sku;
        QString name;
        QString unit;
    };

    explicit ProductComboDelegate(QObject* parent = nullptr);

    void setProducts(const QList<ProductItem>& products);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    QList<ProductItem> m_products;
};
