#pragma once

#include <QAbstractTableModel>
#include <QList>

#include "../employee.h"

class EmployeeTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EmployeeTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setEmployees(const QList<Employee>& employees);

    Employee getEmployee(int row) const;

private:
    QList<Employee> m_employees;
};
