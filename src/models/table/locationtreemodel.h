#pragma once

#include <QAbstractItemModel>
#include <QHash>
#include <QVector>

#include "../location.h"

class LocationTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit LocationTreeModel(QObject* parent = nullptr);
    ~LocationTreeModel() override;

    void setLocations(const QList<Location>& locations);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int locationIdForIndex(const QModelIndex& index) const;
    QString locationPathForIndex(const QModelIndex& index) const;

    QModelIndex indexForLocationId(int locationId) const;

private:
    struct Node {
        Location loc;
        Node* parent{nullptr};
        QVector<Node*> children;
    };

    Node* m_root{nullptr};
    QHash<int, Node*> m_byId;

    void clear();
    static void deleteNode(Node* n);
};
