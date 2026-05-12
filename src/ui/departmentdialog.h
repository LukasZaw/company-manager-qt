#pragma once

#include <QDialog>

#include "../models/table/departmenttablemodel.h"

namespace Ui {
class DepartmentDialog;
}

class DepartmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepartmentDialog(QWidget* parent = nullptr);
    ~DepartmentDialog();

private slots:
    void on_addDepartmentButton_clicked();
    void on_refreshButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::DepartmentDialog* ui;
    DepartmentTableModel* m_model;

    void loadDepartments();
};
