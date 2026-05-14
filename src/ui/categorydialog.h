#pragma once

#include <QDialog>

#include "../models/table/categorytablemodel.h"

namespace Ui {
class CategoryDialog;
}

class CategoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CategoryDialog(QWidget* parent = nullptr);
    ~CategoryDialog();

private slots:
    void on_addCategoryButton_clicked();
    void on_refreshButton_clicked();
    void on_renameButton_clicked();
    void on_deleteButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::CategoryDialog* ui;
    CategoryTableModel* m_model;

    void loadCategories();
};
