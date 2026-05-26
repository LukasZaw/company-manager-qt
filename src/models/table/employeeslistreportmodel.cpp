#include "employeeslistreportmodel.h"

#include "../../services/employeeservice.h"

EmployeesListReportModel::EmployeesListReportModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    reload();
}

int EmployeesListReportModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_employees.size();
}

int EmployeesListReportModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant EmployeesListReportModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const int row = index.row();
    if (row < 0 || row >= m_employees.size())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const Employee& e = m_employees.at(row);

    switch (index.column()) {
    case Id:
        return e.id;
    case FirstName:
        return e.firstName;
    case LastName:
        return e.lastName;
    case Email:
        return e.email;
    case Phone:
        return e.phone;
    case Position:
        return e.position;
    case Department:
        return e.department;
    case HireDate:
        return e.hireDate.isValid() ? e.hireDate.toString("yyyy-MM-dd") : QString();
    case Active:
        return e.active ? tr("Aktywny") : tr("Nieaktywny");
    case Notes:
        return e.notes;
    default:
        return {};
    }
}

QVariant EmployeesListReportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (section) {
    case Id:
        return tr("ID");
    case FirstName:
        return tr("Imię");
    case LastName:
        return tr("Nazwisko");
    case Email:
        return tr("Email");
    case Phone:
        return tr("Telefon");
    case Position:
        return tr("Stanowisko");
    case Department:
        return tr("Dział");
    case HireDate:
        return tr("Data zatrudnienia");
    case Active:
        return tr("Status");
    case Notes:
        return tr("Notatki");
    default:
        return {};
    }
}

void EmployeesListReportModel::reload()
{
    beginResetModel();
    m_employees = EmployeeService::getAllEmployees();
    endResetModel();
}

QList<Employee> EmployeesListReportModel::employees() const
{
    return m_employees;
}
