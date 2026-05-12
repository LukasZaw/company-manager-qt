#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/models/table/employeetablemodel.h"

class QModelIndex;

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addEmployeeButton_clicked();

    void on_deleteEmployeeButton_clicked();

    void on_editEmployeeButton_clicked();

    void on_employeesTableView_doubleClicked(const QModelIndex& index);

    void on_manageDepartmentsButton_clicked();

    void on_actionDepartments_triggered();

private:
    Ui::MainWindow *ui;
    EmployeeTableModel* employeeModel;

    void loadEmployees();
    void editEmployeeAtRow(int row);
};
#endif // MAINWINDOW_H
