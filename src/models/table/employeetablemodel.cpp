#include "EmployeeTableModel.h"

EmployeeTableModel::EmployeeTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int EmployeeTableModel::rowCount(const QModelIndex&) const {
    return m_employees.size();
}

int EmployeeTableModel::columnCount( const QModelIndex&) const {
    return 9; // ID, Imię, Nazwisko, Email, Telefon, Stanowisko, Dział, Data zatrudnienia, Status
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

    case 3:
        return employee.email;

    case 4:
        return employee.phone;

    case 5:
        return employee.position;

    case 6:
        return employee.department;

    case 7:
        return employee.hireDate.isValid() ? employee.hireDate.toString(Qt::ISODate) : QString();

    case 8:
        return employee.active ? QObject::tr("Aktywny") : QObject::tr("Nieaktywny");

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

    case 3:
        return "Email";

    case 4:
        return "Telefon";

    case 5:
        return "Stanowisko";

    case 6:
        return "Dział";

    case 7:
        return "Data zatrudnienia";

    case 8:
        return "Status";

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
    if (row < 0 || row >= m_employees.size())
        return Employee{};

    return m_employees.at(row);
}
