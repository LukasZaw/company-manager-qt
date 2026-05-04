#include "EmployeeTableModel.h"

EmployeeTableModel::EmployeeTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int EmployeeTableModel::rowCount(const QModelIndex&) const {
    return m_employees.size();
}

int EmployeeTableModel::columnCount( const QModelIndex&) const {
    return 3;
}

QVariant EmployeeTableModel::data(const QModelIndex& index, int role) const {

    if (!index.isValid())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const Employee& employee =
        m_employees[index.row()];

    switch(index.column()) {

    case 0:
        return employee.id;

    case 1:
        return employee.firstName;

    case 2:
        return employee.lastName;

    default:
        return {};
    }
}

QVariant EmployeeTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch(section) {

    case 0:
        return "ID";

    case 1:
        return "Imię";

    case 2:
        return "Nazwisko";

    default:
        return {};
    }
}

void EmployeeTableModel::setEmployees(const QList<Employee>& employees) {
    beginResetModel();

    m_employees = employees;

    endResetModel();
}

Employee EmployeeTableModel::getEmployee(int row) const {

    return m_employees[row];
}
