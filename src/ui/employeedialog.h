#ifndef EMPLOYEEDIALOG_H
#define EMPLOYEEDIALOG_H
#include "../models/employee.h"

#include <QDialog>

namespace Ui {
class EmployeeDialog;
}

class EmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EmployeeDialog(QWidget *parent = nullptr);
    ~EmployeeDialog();

    Employee getEmployee() const;

private slots:
    void on_saveButton_clicked();

private:
    Ui::EmployeeDialog *ui;
};

#endif // EMPLOYEEDIALOG_H
