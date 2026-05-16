#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/models/table/employeetablemodel.h"
#include "src/models/table/producttablemodel.h"

class QModelIndex;
class QSortFilterProxyModel;
class StockMovementsListModel;
class StockMovementLinesModel;
class MovementCardDelegate;
class ProductComboDelegate;
enum class MovementType;

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

    void on_actionLocations_triggered();

    void on_warehouseAddLineButton_clicked();
    void on_warehouseRemoveLineButton_clicked();

    void on_warehousePostButton_clicked();
    void on_warehouseCancelMovementButton_clicked();

    void on_warehouseReceiptButton_clicked();
    void on_warehouseIssueButton_clicked();
    void on_warehouseRelocateButton_clicked();
    void on_warehouseAdjustButton_clicked();

    void on_warehouseFromLocationPickButton_clicked();
    void on_warehouseToLocationPickButton_clicked();

private:
    Ui::MainWindow *ui;
    EmployeeTableModel* employeeModel;
    QSortFilterProxyModel* employeeProxyModel;

    ProductTableModel* productModel;
    QSortFilterProxyModel* productProxyModel;

    // Warehouse
    StockMovementsListModel* warehouseMovementsModel{nullptr};
    StockMovementLinesModel* warehouseLinesModel{nullptr};
    MovementCardDelegate* warehouseMovementsDelegate{nullptr};
    ProductComboDelegate* warehouseProductDelegate{nullptr};
    int currentWarehouseMovementId{0};
    bool warehouseIsEditingNew{false};
    MovementType currentWarehouseMovementType;
    int warehouseFromLocationId{0};
    int warehouseToLocationId{0};

    void loadEmployees();
    void loadProducts();
    void editEmployeeAtRow(int row);

    void editProductAtRow(int row);

    void initWarehouseUi();
    void loadWarehouseMovements();
    void showWarehouseMovement(int movementId);
    void showWarehouseEmpty();
    void startNewWarehouseMovement(MovementType type);

    void refreshWarehouseEmployees();
    void refreshWarehouseProductCatalog();
};
#endif // MAINWINDOW_H
