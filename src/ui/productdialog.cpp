#include "productdialog.h"
#include "ui_productdialog.h"

#include "../services/categoryservice.h"

#include <QMessageBox>

ProductDialog::ProductDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ProductDialog)
{
    ui->setupUi(this);

    connect(ui->cancleButton, &QPushButton::clicked, this, &QDialog::reject);

    // Populate categories
    const auto categories = CategoryService::getAllCategories();
    ui->categoryComboBox->clear();
    for (const auto& c : categories) {
        ui->categoryComboBox->addItem(c.name, c.id);
    }

    ui->priceSpinBox->setDecimals(2);
    ui->priceSpinBox->setMaximum(1e9);
    ui->quantitySpinBox->setDecimals(3);
    ui->quantitySpinBox->setMaximum(1e9);
}

ProductDialog::~ProductDialog()
{
    delete ui;
}

Product ProductDialog::getProduct() const
{
    Product p;
    p.id = m_productId;

    p.name = ui->nameEdit->text();
    p.sku = ui->skuEdit->text();
    p.categoryId = ui->categoryComboBox->currentData().toInt();
    p.category = ui->categoryComboBox->currentText();
    p.price = ui->priceSpinBox->value();
    p.quantity = ui->quantitySpinBox->value();
    p.unit = ui->unitEdit->text();
    p.location = ui->locationEdit->text();
    p.description = ui->descriptionEdit->toPlainText();

    return p;
}

void ProductDialog::on_saveButton_clicked()
{
    if (ui->nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nazwa produktu jest wymagana."));
        return;
    }

    if (ui->skuEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Kod produktu (SKU) jest wymagany."));
        return;
    }

    if (ui->categoryComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Wybierz kategorię."));
        return;
    }

    accept();
}

void ProductDialog::setProduct(const Product& product)
{
    m_productId = product.id;

    ui->nameEdit->setText(product.name);
    ui->skuEdit->setText(product.sku);

    // Select category by id if possible
    int idx = -1;
    for (int i = 0; i < ui->categoryComboBox->count(); ++i) {
        if (ui->categoryComboBox->itemData(i).toInt() == product.categoryId) {
            idx = i;
            break;
        }
    }
    if (idx >= 0)
        ui->categoryComboBox->setCurrentIndex(idx);

    ui->priceSpinBox->setValue(product.price);
    ui->quantitySpinBox->setValue(product.quantity);
    ui->unitEdit->setText(product.unit);
    ui->locationEdit->setText(product.location);
    ui->descriptionEdit->setPlainText(product.description);
}
