#pragma once

#include <QDialog>

class QPushButton;
class QTreeView;

class LocationTreeModel;

class LocationManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocationManagerDialog(QWidget* parent = nullptr);
    ~LocationManagerDialog() override;

private slots:
    void onSelectionChanged();
    void onAddChildClicked();
    void onRenameClicked();
    void onDeleteClicked();

private:
    LocationTreeModel* m_model{nullptr};
    QTreeView* m_tree{nullptr};

    QPushButton* m_addChild{nullptr};
    QPushButton* m_rename{nullptr};
    QPushButton* m_delete{nullptr};
    QPushButton* m_close{nullptr};

    void reloadAndSelect(int locationIdToSelect = 0);
    int currentLocationId() const;
};
