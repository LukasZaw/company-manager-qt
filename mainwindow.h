#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/models/table/employeetablemodel.h"
#include "src/models/table/producttablemodel.h"

class QModelIndex;
class QSortFilterProxyModel;

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

    void on_addProductButton_clicked();

    void on_editProductButton_clicked();

    void on_deleteProductButton_clicked();

    void on_productsTableView_doubleClicked(const QModelIndex& index);

    void on_manageCategoriesButton_clicked();

    void on_actionCategories_triggered();

private:
    Ui::MainWindow *ui;
    EmployeeTableModel* employeeModel;
    QSortFilterProxyModel* employeeProxyModel;

    ProductTableModel* productModel;
    QSortFilterProxyModel* productProxyModel;

    void loadEmployees();
    void loadProducts();
    void editEmployeeAtRow(int row);

    void editProductAtRow(int row);
};
#endif // MAINWINDOW_H
