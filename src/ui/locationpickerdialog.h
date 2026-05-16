#pragma once

#include <QDialog>

class QTreeView;
class QPushButton;

class LocationTreeModel;

class LocationPickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocationPickerDialog(QWidget* parent = nullptr);
    ~LocationPickerDialog() override;

    int selectedLocationId() const;
    QString selectedLocationPath() const;

    void setCurrentLocationId(int locationId);

private slots:
    void onSelectionChanged();

private:
    LocationTreeModel* m_model{nullptr};
    QTreeView* m_tree{nullptr};
    QPushButton* m_ok{nullptr};
    int m_selectedId{0};
    QString m_selectedPath;
};
