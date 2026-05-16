#include "locationtreemodel.h"

LocationTreeModel::LocationTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    m_root = new Node();
}

LocationTreeModel::~LocationTreeModel()
{
    clear();
    delete m_root;
    m_root = nullptr;
}

void LocationTreeModel::deleteNode(Node* n)
{
    if (!n)
        return;
    for (Node* c : n->children)
        deleteNode(c);
    delete n;
}

void LocationTreeModel::clear()
{
    beginResetModel();
    m_byId.clear();

    if (m_root) {
        for (Node* c : m_root->children)
            deleteNode(c);
        m_root->children.clear();
    }

    endResetModel();
}

void LocationTreeModel::setLocations(const QList<Location>& locations)
{
    beginResetModel();

    m_byId.clear();
    if (m_root) {
        for (Node* c : m_root->children)
            deleteNode(c);
        m_root->children.clear();
    }

    // Create nodes
    for (const auto& l : locations) {
        Node* n = new Node();
        n->loc = l;
        m_byId.insert(l.id, n);
    }

    // Link into tree
    for (auto it = m_byId.begin(); it != m_byId.end(); ++it) {
        Node* n = it.value();
        Node* parent = nullptr;
        if (n->loc.parentId > 0)
            parent = m_byId.value(n->loc.parentId, nullptr);
        if (!parent)
            parent = m_root;

        n->parent = parent;
        parent->children.append(n);
    }

    endResetModel();
}

int LocationTreeModel::columnCount(const QModelIndex&) const
{
    return 1;
}

int LocationTreeModel::rowCount(const QModelIndex& parent) const
{
    const Node* p = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : m_root;
    if (!p)
        return 0;
    return p->children.size();
}

QModelIndex LocationTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column != 0)
        return QModelIndex();

    const Node* p = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : m_root;
    if (!p)
        return QModelIndex();

    if (row < 0 || row >= p->children.size())
        return QModelIndex();

    Node* child = p->children.at(row);
    return createIndex(row, column, child);
}

QModelIndex LocationTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    Node* n = static_cast<Node*>(child.internalPointer());
    if (!n || !n->parent || n->parent == m_root)
        return QModelIndex();

    Node* p = n->parent;
    Node* gp = p->parent ? p->parent : m_root;

    int row = 0;
    for (int i = 0; i < gp->children.size(); ++i) {
        if (gp->children.at(i) == p) {
            row = i;
            break;
        }
    }

    return createIndex(row, 0, p);
}

QVariant LocationTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    Node* n = static_cast<Node*>(index.internalPointer());
    if (!n)
        return {};

    if (role == Qt::DisplayRole)
        return n->loc.name;

    if (role == Qt::ToolTipRole)
        return n->loc.path;

    return {};
}

int LocationTreeModel::locationIdForIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;
    Node* n = static_cast<Node*>(index.internalPointer());
    return n ? n->loc.id : 0;
}

QString LocationTreeModel::locationPathForIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return QString();
    Node* n = static_cast<Node*>(index.internalPointer());
    return n ? n->loc.path : QString();
}

QModelIndex LocationTreeModel::indexForLocationId(int locationId) const
{
    Node* n = m_byId.value(locationId, nullptr);
    if (!n || !n->parent)
        return QModelIndex();

    Node* p = n->parent;
    int row = 0;
    for (int i = 0; i < p->children.size(); ++i) {
        if (p->children.at(i) == n) {
            row = i;
            break;
        }
    }

    return createIndex(row, 0, n);
}
