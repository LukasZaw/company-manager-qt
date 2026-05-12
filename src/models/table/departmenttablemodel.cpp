#include "departmenttablemodel.h"

DepartmentTableModel::DepartmentTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int DepartmentTableModel::rowCount(const QModelIndex&) const {
    return m_departments.size();
}

int DepartmentTableModel::columnCount(const QModelIndex&) const {
    return 2; // ID, Nazwa
}

QVariant DepartmentTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    if (index.row() < 0 || index.row() >= m_departments.size())
        return {};

    const auto& department = m_departments.at(index.row());

    switch (index.column()) {
    case 0:
        return department.id;
    case 1:
        return department.name;
    default:
        return {};
    }
}

QVariant DepartmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

void DepartmentTableModel::setDepartments(const QList<Department>& departments) {
    beginResetModel();
    m_departments = departments;
    endResetModel();
}

Department DepartmentTableModel::getDepartment(int row) const {
    if (row < 0 || row >= m_departments.size())
        return Department{};

    return m_departments.at(row);
}
