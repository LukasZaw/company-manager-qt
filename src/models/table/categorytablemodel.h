#pragma once

#include <QAbstractTableModel>
#include <QList>

#include "../category.h"

class CategoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CategoryTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setCategories(const QList<Category>& categories);
    Category getCategory(int row) const;

private:
    QList<Category> m_categories;
};
