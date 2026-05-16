#include "locationpickerdialog.h"

#include "../models/table/locationtreemodel.h"
#include "../services/locationservice.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

LocationPickerDialog::LocationPickerDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Wybierz lokalizację"));
    resize(520, 520);

    m_model = new LocationTreeModel(this);
    m_model->setLocations(LocationService::getAllLocations());

    m_tree = new QTreeView(this);
    m_tree->setModel(m_model);
    m_tree->setHeaderHidden(true);
    m_tree->setUniformRowHeights(true);
    m_tree->expandAll();

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_ok = buttons->button(QDialogButtonBox::Ok);
    m_ok->setEnabled(false);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(m_tree->selectionModel(), &QItemSelectionModel::selectionChanged, this, &LocationPickerDialog::onSelectionChanged);
    connect(m_tree, &QTreeView::doubleClicked, this, [this](const QModelIndex& idx) {
        m_selectedId = m_model->locationIdForIndex(idx);
        m_selectedPath = m_model->locationPathForIndex(idx);
        if (m_selectedId > 0)
            accept();
    });

    auto* root = new QVBoxLayout();
    root->addWidget(m_tree);
    root->addWidget(buttons);
    setLayout(root);
}

LocationPickerDialog::~LocationPickerDialog() = default;

int LocationPickerDialog::selectedLocationId() const
{
    return m_selectedId;
}

QString LocationPickerDialog::selectedLocationPath() const
{
    return m_selectedPath;
}

void LocationPickerDialog::setCurrentLocationId(int locationId)
{
    const QModelIndex idx = m_model->indexForLocationId(locationId);
    if (!idx.isValid())
        return;

    m_tree->setCurrentIndex(idx);
    m_tree->scrollTo(idx);
}

void LocationPickerDialog::onSelectionChanged()
{
    const QModelIndex idx = m_tree->currentIndex();
    m_selectedId = m_model->locationIdForIndex(idx);
    m_selectedPath = m_model->locationPathForIndex(idx);
    m_ok->setEnabled(m_selectedId > 0);
}
