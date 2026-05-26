#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "src/services/employeeservice.h"
#include "src/services/productservice.h"
#include "src/services/stockmovementservice.h"
#include "src/models/stockmovement.h"
#include "src/models/table/stockmovementslistmodel.h"
#include "src/models/table/stockmovementlinesmodel.h"
#include "src/ui/employeedialog.h"
#include "src/ui/productdialog.h"
#include "src/ui/departmentdialog.h"
#include "src/ui/categorydialog.h"
#include "src/ui/movementcarddelegate.h"
#include "src/ui/productcombodelegate.h"
#include "src/ui/locationpickerdialog.h"
#include "src/ui/locationmanagerdialog.h"
#include "src/services/locationservice.h"
#include "src/models/table/currentstockreportmodel.h"
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QSortFilterProxyModel>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    employeeModel = new EmployeeTableModel(this);

    employeeProxyModel = new QSortFilterProxyModel(this);
    employeeProxyModel->setSourceModel(employeeModel);
    employeeProxyModel->setDynamicSortFilter(true);
    employeeProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    employeeProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    employeeProxyModel->setFilterKeyColumn(-1);

    ui->employeesTableView->setModel(employeeProxyModel);

    ui->employeesTableView
        ->setSelectionBehavior(
            QAbstractItemView::SelectRows
            );

    ui->employeesTableView
        ->setSelectionMode(
            QAbstractItemView::SingleSelection
            );

    ui->employeesTableView
        ->setSortingEnabled(true);

    ui->employeesTableView
        ->horizontalHeader()
        ->setStretchLastSection(true);

    connect(ui->employeeFilterLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        const QString trimmed = text.trimmed();
        if (trimmed.isEmpty()) {
            employeeProxyModel->setFilterRegularExpression(QRegularExpression());
            return;
        }

        const QRegularExpression rx(
            QRegularExpression::escape(trimmed),
            QRegularExpression::CaseInsensitiveOption
        );
        employeeProxyModel->setFilterRegularExpression(rx);
    });

    productModel = new ProductTableModel(this);

    productProxyModel = new QSortFilterProxyModel(this);
    productProxyModel->setSourceModel(productModel);
    productProxyModel->setDynamicSortFilter(true);
    productProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    productProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    productProxyModel->setFilterKeyColumn(-1);

    ui->productsTableView->setModel(productProxyModel);
    ui->productsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->productsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->productsTableView->setSortingEnabled(true);
    ui->productsTableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->productFilterLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        const QString trimmed = text.trimmed();
        if (trimmed.isEmpty()) {
            productProxyModel->setFilterRegularExpression(QRegularExpression());
            return;
        }

        const QRegularExpression rx(
            QRegularExpression::escape(trimmed),
            QRegularExpression::CaseInsensitiveOption
        );
        productProxyModel->setFilterRegularExpression(rx);
    });

    loadEmployees();
    loadProducts();

    initReportsUi();

    initWarehouseUi();
    loadWarehouseMovements();
    showWarehouseEmpty();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadEmployees()
{
    auto employees =
        EmployeeService::getAllEmployees();

    employeeModel
        ->setEmployees(employees);

    refreshWarehouseEmployees();
}

void MainWindow::loadProducts()
{
    const auto products = ProductService::getAllProducts();
    productModel->setProducts(products);

    refreshWarehouseProductCatalog();
}

void MainWindow::on_addEmployeeButton_clicked()
{
    EmployeeDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        Employee employee = dialog.getEmployee();

        EmployeeService::addEmployee(employee);

        loadEmployees();
    }

}

void MainWindow::on_deleteEmployeeButton_clicked()
{
    QModelIndex index =
        ui->employeesTableView
            ->currentIndex();

    if (!index.isValid())
        return;

    const QModelIndex sourceIndex = employeeProxyModel->mapToSource(index);
    Employee employee = employeeModel->getEmployee(sourceIndex.row());

    EmployeeService::deleteEmployee(employee.id);

    loadEmployees();
}

void MainWindow::on_editEmployeeButton_clicked()
{
    const QModelIndex index = ui->employeesTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz pracownika z listy."));
        return;
    }

    const QModelIndex sourceIndex = employeeProxyModel->mapToSource(index);
    editEmployeeAtRow(sourceIndex.row());
}

void MainWindow::on_employeesTableView_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    const QModelIndex sourceIndex = employeeProxyModel->mapToSource(index);
    editEmployeeAtRow(sourceIndex.row());
}

void MainWindow::editEmployeeAtRow(int row)
{
    const Employee employee = employeeModel->getEmployee(row);
    if (employee.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się odczytać pracownika."));
        return;
    }

    EmployeeDialog dialog(this);
    dialog.setEmployee(employee);

    if (dialog.exec() == QDialog::Accepted) {
        const Employee updated = dialog.getEmployee();
        if (!EmployeeService::updateEmployee(updated)) {
            QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zapisać zmian."));
            return;
        }

        loadEmployees();
    }
}

void MainWindow::on_manageDepartmentsButton_clicked()
{
    on_actionDepartments_triggered();
}

void MainWindow::on_actionDepartments_triggered()
{
    DepartmentDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_addProductButton_clicked()
{
    ProductDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        const Product product = dialog.getProduct();
        if (!ProductService::addProduct(product)) {
            QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się dodać produktu. Sprawdź SKU oraz kategorię."));
            return;
        }
        loadProducts();
    }
}

void MainWindow::on_deleteProductButton_clicked()
{
    const QModelIndex index = ui->productsTableView->currentIndex();
    if (!index.isValid())
        return;

    const QModelIndex sourceIndex = productProxyModel->mapToSource(index);
    const Product product = productModel->getProduct(sourceIndex.row());
    if (product.id <= 0)
        return;

    if (!ProductService::deleteProduct(product.id)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się usunąć produktu."));
        return;
    }

    loadProducts();
}

void MainWindow::on_editProductButton_clicked()
{
    const QModelIndex index = ui->productsTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::information(this, tr("Informacja"), tr("Wybierz produkt z listy."));
        return;
    }

    const QModelIndex sourceIndex = productProxyModel->mapToSource(index);
    editProductAtRow(sourceIndex.row());
}

void MainWindow::on_productsTableView_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    const QModelIndex sourceIndex = productProxyModel->mapToSource(index);
    editProductAtRow(sourceIndex.row());
}

void MainWindow::editProductAtRow(int row)
{
    const Product product = productModel->getProduct(row);
    if (product.id <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się odczytać produktu."));
        return;
    }

    ProductDialog dialog(this);
    dialog.setProduct(product);

    if (dialog.exec() == QDialog::Accepted) {
        const Product updated = dialog.getProduct();
        if (!ProductService::updateProduct(updated)) {
            QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zapisać zmian. Sprawdź SKU oraz kategorię."));
            return;
        }
        loadProducts();
    }
}

void MainWindow::on_manageCategoriesButton_clicked()
{
    on_actionCategories_triggered();
}

void MainWindow::on_actionCategories_triggered()
{
    CategoryDialog dialog(this);
    dialog.exec();

    loadProducts();
}

void MainWindow::on_actionLocations_triggered()
{
    LocationManagerDialog dialog(this);
    dialog.exec();

    loadProducts();
    loadWarehouseMovements();

    if (warehouseIsEditingNew) {
        if (warehouseFromLocationId > 0)
            ui->warehouseFromLocationEdit->setText(LocationService::getPathById(warehouseFromLocationId));
        if (warehouseToLocationId > 0)
            ui->warehouseToLocationEdit->setText(LocationService::getPathById(warehouseToLocationId));
        return;
    }

    if (currentWarehouseMovementId > 0)
        showWarehouseMovement(currentWarehouseMovementId);
    else
        showWarehouseEmpty();
}

void MainWindow::initWarehouseUi()
{
    warehouseMovementsModel = new StockMovementsListModel(this);
    warehouseLinesModel = new StockMovementLinesModel(this);

    ui->warehouseMovementsListView->setModel(warehouseMovementsModel);
    ui->warehouseMovementsListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->warehouseMovementsListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->warehouseMovementsListView->setUniformItemSizes(false);

    warehouseMovementsDelegate = new MovementCardDelegate(ui->warehouseMovementsListView);
    ui->warehouseMovementsListView->setItemDelegate(warehouseMovementsDelegate);

    connect(ui->warehouseSearchLineEdit, &QLineEdit::textChanged, this, [this](const QString&) {
        loadWarehouseMovements();
    });

    connect(ui->warehouseMovementsListView->selectionModel(), &QItemSelectionModel::currentChanged, this,
        [this](const QModelIndex& current, const QModelIndex&) {
            if (!current.isValid()) {
                showWarehouseEmpty();
                return;
            }

            const int id = current.data(StockMovementsListModel::IdRole).toInt();
            showWarehouseMovement(id);
        });

    ui->warehouseLinesTableView->setModel(warehouseLinesModel);
    ui->warehouseLinesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->warehouseLinesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->warehouseLinesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->warehouseLinesTableView->setSortingEnabled(false);

    ui->warehouseOccurredAtEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    ui->warehouseOccurredAtEdit->setCalendarPopup(true);

    warehouseProductDelegate = new ProductComboDelegate(ui->warehouseLinesTableView);
    ui->warehouseLinesTableView->setItemDelegateForColumn(StockMovementLinesModel::Product, warehouseProductDelegate);

    refreshWarehouseEmployees();
    refreshWarehouseProductCatalog();

    warehouseIsEditingNew = false;
    ui->warehousePostButton->setEnabled(false);
    ui->warehouseCancelMovementButton->setEnabled(false);
    ui->warehouseAddLineButton->setEnabled(false);
    ui->warehouseRemoveLineButton->setEnabled(false);
    ui->warehouseLinesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

static QString csvEscape(const QString& value)
{
    QString v = value;
    const bool needsQuotes = v.contains(';') || v.contains('"') || v.contains('\n') || v.contains('\r');
    v.replace('"', "\"\"");
    if (needsQuotes)
        return '"' + v + '"';
    return v;
}

void MainWindow::initReportsUi()
{
    // Left: report selector
    ui->reportsListWidget->clear();

    auto* currentStockItem = new QListWidgetItem(tr("Aktualny stan magazynu"));
    currentStockItem->setData(Qt::UserRole, 0); // page index
    ui->reportsListWidget->addItem(currentStockItem);

    auto* receiptsHistoryItem = new QListWidgetItem(tr("Historia przyjęć (wkrótce)"));
    receiptsHistoryItem->setData(Qt::UserRole, 1);
    receiptsHistoryItem->setFlags(receiptsHistoryItem->flags() & ~Qt::ItemIsEnabled);
    ui->reportsListWidget->addItem(receiptsHistoryItem);

    auto* employeesListItem = new QListWidgetItem(tr("Lista pracowników (wkrótce)"));
    employeesListItem->setData(Qt::UserRole, 1);
    employeesListItem->setFlags(employeesListItem->flags() & ~Qt::ItemIsEnabled);
    ui->reportsListWidget->addItem(employeesListItem);

    // Right: current stock preview
    currentStockReportModel = new CurrentStockReportModel(this);
    ui->currentStockTableView->setModel(currentStockReportModel);
    ui->currentStockTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->currentStockTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->currentStockTableView->horizontalHeader()->setStretchLastSection(true);
    ui->currentStockTableView->setSortingEnabled(false);

    ui->reportsStackedWidget->setCurrentIndex(0);
    ui->reportsListWidget->setCurrentRow(0);
}

void MainWindow::on_reportsListWidget_currentRowChanged(int currentRow)
{
    if (currentRow < 0)
        return;

    auto* item = ui->reportsListWidget->item(currentRow);
    if (!item)
        return;

    const int pageIndex = item->data(Qt::UserRole).toInt();
    ui->reportsStackedWidget->setCurrentIndex(pageIndex);

    if (pageIndex == 0 && currentStockReportModel)
        currentStockReportModel->reload();
}

void MainWindow::on_exportCurrentStockCsvButton_clicked()
{
    if (!currentStockReportModel)
        return;

    currentStockReportModel->reload();
    const auto products = currentStockReportModel->products();

    const QString suggested = QString("stan_magazynu_%1.csv").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Eksportuj CSV"),
        suggested,
        tr("CSV (*.csv)"));

    if (filePath.trimmed().isEmpty())
        return;

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zapisać pliku."));
        return;
    }

    // UTF-8 BOM (helps Excel on Windows)
    f.write("\xEF\xBB\xBF", 3);

    QTextStream out(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#endif

    out << "SKU;Produkt;Stan;Jedn.;Lokalizacja\n";

    const QLocale loc;
    for (const auto& p : products) {
        const QString qty = loc.toString(p.quantity);
        out << csvEscape(p.sku) << ';'
            << csvEscape(p.name) << ';'
            << csvEscape(qty) << ';'
            << csvEscape(p.unit) << ';'
            << csvEscape(p.location) << "\n";
    }

    out.flush();
    f.close();

    QMessageBox::information(this, tr("Gotowe"), tr("Zapisano raport do pliku CSV."));
}

void MainWindow::refreshWarehouseEmployees()
{
    if (!ui || !ui->warehouseEmployeeComboBox)
        return;

    const int currentId = ui->warehouseEmployeeComboBox->currentData().toInt();

    const QSignalBlocker blocker(ui->warehouseEmployeeComboBox);
    ui->warehouseEmployeeComboBox->clear();
    ui->warehouseEmployeeComboBox->addItem(tr("(brak)"), 0);

    const auto employees = EmployeeService::getAllEmployees();
    for (const auto& e : employees) {
        const QString name = (e.firstName.trimmed() + " " + e.lastName.trimmed()).trimmed();
        ui->warehouseEmployeeComboBox->addItem(name, e.id);
    }

    int idx = 0;
    if (currentId > 0) {
        for (int i = 0; i < ui->warehouseEmployeeComboBox->count(); ++i) {
            if (ui->warehouseEmployeeComboBox->itemData(i).toInt() == currentId) {
                idx = i;
                break;
            }
        }
    }
    ui->warehouseEmployeeComboBox->setCurrentIndex(idx);
}

void MainWindow::refreshWarehouseProductCatalog()
{
    if (!warehouseLinesModel || !warehouseProductDelegate)
        return;

    QHash<int, StockMovementLinesModel::ProductCatalogItem> catalog;
    QList<ProductComboDelegate::ProductItem> products;

    const auto allProducts = ProductService::getAllProducts();
    catalog.reserve(allProducts.size());
    products.reserve(allProducts.size());

    for (const auto& p : allProducts) {
        StockMovementLinesModel::ProductCatalogItem item;
        item.sku = p.sku;
        item.name = p.name;
        item.unit = p.unit;
        catalog.insert(p.id, item);

        ProductComboDelegate::ProductItem pi;
        pi.id = p.id;
        pi.sku = p.sku;
        pi.name = p.name;
        pi.unit = p.unit;
        products.append(pi);
    }

    warehouseLinesModel->setProductCatalog(catalog);
    warehouseProductDelegate->setProducts(products);

    if (ui && ui->warehouseLinesTableView)
        ui->warehouseLinesTableView->viewport()->update();
}

void MainWindow::loadWarehouseMovements()
{
    const QString search = ui->warehouseSearchLineEdit->text().trimmed();
    const auto movements = StockMovementService::getMovements(search);

    warehouseMovementsModel->setMovements(movements);

    int rowToSelect = -1;
    if (currentWarehouseMovementId > 0) {
        for (int row = 0; row < movements.size(); ++row) {
            if (movements.at(row).id == currentWarehouseMovementId) {
                rowToSelect = row;
                break;
            }
        }
    }

    if (rowToSelect >= 0) {
        const QModelIndex idx = warehouseMovementsModel->index(rowToSelect, 0);
        ui->warehouseMovementsListView->setCurrentIndex(idx);
        return;
    }

    if (!movements.isEmpty()) {
        ui->warehouseMovementsListView->setCurrentIndex(warehouseMovementsModel->index(0, 0));
        return;
    }

    showWarehouseEmpty();
}

void MainWindow::showWarehouseEmpty()
{
    currentWarehouseMovementId = 0;
    warehouseIsEditingNew = false;
    warehouseFromLocationId = 0;
    warehouseToLocationId = 0;
    ui->warehouseRightStack->setCurrentWidget(ui->warehouseEmptyPage);
    warehouseLinesModel->setLines({});

    const QSignalBlocker b1(ui->warehouseOccurredAtEdit);
    const QSignalBlocker b2(ui->warehouseEmployeeComboBox);
    const QSignalBlocker b3(ui->warehouseFromLocationEdit);
    const QSignalBlocker b4(ui->warehouseToLocationEdit);
    const QSignalBlocker b5(ui->warehouseNotesEdit);

    ui->warehouseOccurredAtEdit->setDateTime(QDateTime::currentDateTime());
    ui->warehouseEmployeeComboBox->setCurrentIndex(0);
    ui->warehouseFromLocationEdit->clear();
    ui->warehouseToLocationEdit->clear();
    ui->warehouseNotesEdit->clear();

    ui->warehousePostButton->setEnabled(false);
    ui->warehouseCancelMovementButton->setEnabled(false);
    ui->warehouseAddLineButton->setEnabled(false);
    ui->warehouseRemoveLineButton->setEnabled(false);
    ui->warehouseLinesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::showWarehouseMovement(int movementId)
{
    if (movementId <= 0) {
        showWarehouseEmpty();
        return;
    }

    const StockMovement m = StockMovementService::getMovementById(movementId);
    if (m.id <= 0) {
        showWarehouseEmpty();
        return;
    }

    currentWarehouseMovementId = m.id;
    warehouseIsEditingNew = false;
    warehouseFromLocationId = m.fromLocationId;
    warehouseToLocationId = m.toLocationId;

    ui->warehouseRightStack->setCurrentWidget(ui->warehouseDetailsPage);

    ui->warehouseMovementTitleLabel->setText(QString("%1 #%2").arg(movementTypeDisplayName(m.type)).arg(m.id));

    QStringList meta;
    if (m.occurredAt.isValid())
        meta << m.occurredAt.toString("yyyy-MM-dd HH:mm");
    if (!m.employeeName.trimmed().isEmpty())
        meta << m.employeeName.trimmed();
    if (m.linesCount > 0)
        meta << QString("Pozycji: %1").arg(m.linesCount);
    if (m.canceled)
        meta << tr("ANULOWANE");
    ui->warehouseMovementMetaLabel->setText(meta.join(" • "));

    {
        const QSignalBlocker b1(ui->warehouseOccurredAtEdit);
        const QSignalBlocker b2(ui->warehouseEmployeeComboBox);
        const QSignalBlocker b3(ui->warehouseFromLocationEdit);
        const QSignalBlocker b4(ui->warehouseToLocationEdit);
        const QSignalBlocker b5(ui->warehouseNotesEdit);

        ui->warehouseOccurredAtEdit->setDateTime(m.occurredAt.isValid() ? m.occurredAt : QDateTime::currentDateTime());

        int employeeIdx = 0;
        if (m.employeeId > 0) {
            for (int i = 0; i < ui->warehouseEmployeeComboBox->count(); ++i) {
                if (ui->warehouseEmployeeComboBox->itemData(i).toInt() == m.employeeId) {
                    employeeIdx = i;
                    break;
                }
            }
        }
        ui->warehouseEmployeeComboBox->setCurrentIndex(employeeIdx);

        ui->warehouseFromLocationEdit->setText(m.fromLocation);
        ui->warehouseToLocationEdit->setText(m.toLocation);
        ui->warehouseNotesEdit->setPlainText(m.notes);
    }

    const auto lines = StockMovementService::getMovementLines(movementId);
    warehouseLinesModel->setLines(lines);

    ui->warehousePostButton->setEnabled(false);
    ui->warehouseAddLineButton->setEnabled(false);
    ui->warehouseRemoveLineButton->setEnabled(false);
    ui->warehouseLinesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->warehouseCancelMovementButton->setEnabled(!m.canceled);
}

void MainWindow::startNewWarehouseMovement(MovementType type)
{
    currentWarehouseMovementType = type;
    warehouseIsEditingNew = true;
    warehouseFromLocationId = 0;
    warehouseToLocationId = 0;

    if (ui->warehouseMovementsListView->selectionModel()) {
        const QSignalBlocker blocker(ui->warehouseMovementsListView->selectionModel());
        ui->warehouseMovementsListView->selectionModel()->clearSelection();
        ui->warehouseMovementsListView->setCurrentIndex(QModelIndex());
    }

    ui->warehouseRightStack->setCurrentWidget(ui->warehouseDetailsPage);
    ui->warehouseMovementTitleLabel->setText(QString("%1").arg(movementTypeDisplayName(type)));
    ui->warehouseMovementMetaLabel->setText(tr("Nowy dokument"));

    currentWarehouseMovementId = 0;
    warehouseLinesModel->setLines({});

    const QSignalBlocker b1(ui->warehouseOccurredAtEdit);
    const QSignalBlocker b2(ui->warehouseEmployeeComboBox);
    const QSignalBlocker b3(ui->warehouseFromLocationEdit);
    const QSignalBlocker b4(ui->warehouseToLocationEdit);
    const QSignalBlocker b5(ui->warehouseNotesEdit);

    ui->warehouseOccurredAtEdit->setDateTime(QDateTime::currentDateTime());
    ui->warehouseEmployeeComboBox->setCurrentIndex(0);
    ui->warehouseFromLocationEdit->clear();
    ui->warehouseToLocationEdit->clear();
    ui->warehouseNotesEdit->clear();

    ui->warehousePostButton->setEnabled(true);
    ui->warehouseCancelMovementButton->setEnabled(false);
    ui->warehouseAddLineButton->setEnabled(true);
    ui->warehouseRemoveLineButton->setEnabled(true);
    ui->warehouseLinesTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

    warehouseLinesModel->setLines({ StockMovementLine{} });
    ui->warehouseLinesTableView->setCurrentIndex(warehouseLinesModel->index(0, 0));
    ui->warehouseLinesTableView->edit(warehouseLinesModel->index(0, 0));
}

void MainWindow::on_warehouseAddLineButton_clicked()
{
    if (!warehouseIsEditingNew)
        return;

    const int row = warehouseLinesModel->rowCount();
    if (!warehouseLinesModel->insertRows(row, 1))
        return;

    const QModelIndex idx = warehouseLinesModel->index(row, 0);
    ui->warehouseLinesTableView->setCurrentIndex(idx);
    ui->warehouseLinesTableView->scrollTo(idx);
    ui->warehouseLinesTableView->edit(idx);
}

void MainWindow::on_warehouseRemoveLineButton_clicked()
{
    if (!warehouseIsEditingNew)
        return;

    const QModelIndex idx = ui->warehouseLinesTableView->currentIndex();
    if (!idx.isValid())
        return;

    warehouseLinesModel->removeRows(idx.row(), 1);
}

void MainWindow::on_warehousePostButton_clicked()
{
    if (!warehouseIsEditingNew)
        return;

    switch (currentWarehouseMovementType) {
    case MovementType::Receipt:
        if (warehouseToLocationId <= 0) {
            QMessageBox::warning(this, tr("Błąd"), tr("Wybierz lokalizację docelową (Do)."));
            return;
        }
        break;
    case MovementType::Issue:
        if (warehouseFromLocationId <= 0) {
            QMessageBox::warning(this, tr("Błąd"), tr("Wybierz lokalizację źródłową (Z)."));
            return;
        }
        break;
    case MovementType::Relocate:
        if (warehouseFromLocationId <= 0 || warehouseToLocationId <= 0) {
            QMessageBox::warning(this, tr("Błąd"), tr("Wybierz lokalizację źródłową (Z) i docelową (Do)."));
            return;
        }
        break;
    case MovementType::Adjust:
        // Location optional
        break;
    }

    StockMovement h;
    h.type = currentWarehouseMovementType;
    h.occurredAt = ui->warehouseOccurredAtEdit->dateTime();
    h.employeeId = ui->warehouseEmployeeComboBox->currentData().toInt();
    h.fromLocationId = warehouseFromLocationId;
    h.toLocationId = warehouseToLocationId;
    h.fromLocation = ui->warehouseFromLocationEdit->text();
    h.toLocation = ui->warehouseToLocationEdit->text();
    h.notes = ui->warehouseNotesEdit->toPlainText();

    const auto lines = warehouseLinesModel->lines();
    if (lines.isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Dodaj przynajmniej jedną pozycję."));
        return;
    }

    for (const auto& l : lines) {
        if (l.productId <= 0) {
            QMessageBox::warning(this, tr("Błąd"), tr("Wybierz produkt w każdej pozycji."));
            return;
        }
        if (l.quantity == 0.0) {
            QMessageBox::warning(this, tr("Błąd"), tr("Ilość nie może być równa 0."));
            return;
        }
    }

    int newId = 0;
    if (!StockMovementService::postMovement(h, lines, &newId)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zaksięgować dokumentu. Sprawdź dane oraz stan magazynu."));
        return;
    }

    warehouseIsEditingNew = false;
    currentWarehouseMovementId = newId;
    loadWarehouseMovements();
    showWarehouseMovement(newId);

    loadProducts();
}

void MainWindow::on_warehouseCancelMovementButton_clicked()
{
    if (warehouseIsEditingNew)
        return;
    if (currentWarehouseMovementId <= 0)
        return;

    if (!StockMovementService::cancelMovement(currentWarehouseMovementId)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się anulować dokumentu."));
        return;
    }

    loadWarehouseMovements();
    showWarehouseMovement(currentWarehouseMovementId);

    loadProducts();
}

void MainWindow::on_warehouseReceiptButton_clicked()
{
    startNewWarehouseMovement(MovementType::Receipt);
}

void MainWindow::on_warehouseIssueButton_clicked()
{
    startNewWarehouseMovement(MovementType::Issue);
}

void MainWindow::on_warehouseRelocateButton_clicked()
{
    startNewWarehouseMovement(MovementType::Relocate);
}

void MainWindow::on_warehouseAdjustButton_clicked()
{
    startNewWarehouseMovement(MovementType::Adjust);
}

void MainWindow::on_warehouseFromLocationPickButton_clicked()
{
    LocationPickerDialog dlg(this);
    dlg.setCurrentLocationId(warehouseFromLocationId);

    if (dlg.exec() != QDialog::Accepted)
        return;

    const int id = dlg.selectedLocationId();
    if (id <= 0)
        return;

    warehouseFromLocationId = id;
    ui->warehouseFromLocationEdit->setText(dlg.selectedLocationPath());
}

void MainWindow::on_warehouseToLocationPickButton_clicked()
{
    LocationPickerDialog dlg(this);
    dlg.setCurrentLocationId(warehouseToLocationId);

    if (dlg.exec() != QDialog::Accepted)
        return;

    const int id = dlg.selectedLocationId();
    if (id <= 0)
        return;

    warehouseToLocationId = id;
    ui->warehouseToLocationEdit->setText(dlg.selectedLocationPath());
}

