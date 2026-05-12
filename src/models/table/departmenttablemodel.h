#pragma once

#include <QAbstractTableModel>
#include <QList>

#include "../department.h"

class DepartmentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DepartmentTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setDepartments(const QList<Department>& departments);
    Department getDepartment(int row) const;

private:
    QList<Department> m_departments;
};
