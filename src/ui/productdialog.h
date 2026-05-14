#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>

#include "../models/product.h"

namespace Ui {
class ProductDialog;
}

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductDialog(QWidget* parent = nullptr);
    ~ProductDialog();

    Product getProduct() const;
    void setProduct(const Product& product);

private slots:
    void on_saveButton_clicked();

private:
    Ui::ProductDialog* ui;
    int m_productId{0};
};

#endif // PRODUCTDIALOG_H
