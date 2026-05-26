#include "locationmanagerdialog.h"

#include "../models/table/locationtreemodel.h"
#include "../services/locationservice.h"

#include <QHBoxLayout>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

static constexpr int kRootLocationId = 1;

LocationManagerDialog::LocationManagerDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Lokalizacje"));
    resize(640, 560);

    m_model = new LocationTreeModel(this);
    m_model->setLocations(LocationService::getAllLocations());

    m_tree = new QTreeView(this);
    m_tree->setModel(m_model);
    m_tree->setHeaderHidden(true);
    m_tree->setUniformRowHeights(true);
    m_tree->expandAll();

    m_addChild = new QPushButton(tr("Dodaj"), this);
    m_rename = new QPushButton(tr("Zmień nazwę"), this);
    m_delete = new QPushButton(tr("Usuń"), this);
    m_close = new QPushButton(tr("Zamknij"), this);

    auto* buttonsRow = new QHBoxLayout();
    buttonsRow->addWidget(m_addChild);
    buttonsRow->addWidget(m_rename);
    buttonsRow->addWidget(m_delete);
    buttonsRow->addStretch();
    buttonsRow->addWidget(m_close);

    auto* root = new QVBoxLayout();
    root->addWidget(m_tree);
    root->addLayout(buttonsRow);
    setLayout(root);

    connect(m_close, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_addChild, &QPushButton::clicked, this, &LocationManagerDialog::onAddChildClicked);
    connect(m_rename, &QPushButton::clicked, this, &LocationManagerDialog::onRenameClicked);
    connect(m_delete, &QPushButton::clicked, this, &LocationManagerDialog::onDeleteClicked);

    connect(m_tree->selectionModel(), &QItemSelectionModel::selectionChanged, this, &LocationManagerDialog::onSelectionChanged);

    connect(m_tree, &QTreeView::doubleClicked, this, [this](const QModelIndex&) {
        onRenameClicked();
    });

    onSelectionChanged();
}

LocationManagerDialog::~LocationManagerDialog() = default;

int LocationManagerDialog::currentLocationId() const
{
    const QModelIndex idx = m_tree->currentIndex();
    return m_model->locationIdForIndex(idx);
}

void LocationManagerDialog::reloadAndSelect(int locationIdToSelect)
{
    m_model->setLocations(LocationService::getAllLocations());
    m_tree->expandAll();

    if (locationIdToSelect > 0) {
        const QModelIndex idx = m_model->indexForLocationId(locationIdToSelect);
        if (idx.isValid()) {
            m_tree->setCurrentIndex(idx);
            m_tree->scrollTo(idx);
        }
    }

    onSelectionChanged();
}

void LocationManagerDialog::onSelectionChanged()
{
    const int id = currentLocationId();
    const bool hasSelection = id > 0;
    const bool isRoot = id == kRootLocationId;

    m_rename->setEnabled(hasSelection && !isRoot);
    m_delete->setEnabled(hasSelection && !isRoot);
}

void LocationManagerDialog::onAddChildClicked()
{
    const int selectedId = currentLocationId();
    const int parentId = (selectedId > 0) ? selectedId : kRootLocationId;

    bool ok = false;
    const QString name = QInputDialog::getText(
        this,
        tr("Nowa lokalizacja"),
        tr("Nazwa (segment ścieżki):"),
        QLineEdit::Normal,
        QString(),
        &ok);

    if (!ok)
        return;

    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty())
        return;

    int newId = 0;
    if (!LocationService::addLocation(parentId, trimmed, &newId) || newId <= 0) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się dodać lokalizacji. Sprawdź czy nazwa jest unikalna w obrębie rodzica."));
        return;
    }

    reloadAndSelect(newId);
}

void LocationManagerDialog::onRenameClicked()
{
    const int id = currentLocationId();
    if (id <= 0 || id == kRootLocationId)
        return;

    const Location current = LocationService::getLocationById(id);
    if (current.id <= 0)
        return;

    bool ok = false;
    const QString name = QInputDialog::getText(
        this,
        tr("Zmień nazwę"),
        tr("Nowa nazwa (segment ścieżki):"),
        QLineEdit::Normal,
        current.name,
        &ok);

    if (!ok)
        return;

    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty())
        return;

    if (!LocationService::renameLocation(id, trimmed)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się zmienić nazwy. Sprawdź czy nazwa jest unikalna w obrębie rodzica."));
        return;
    }

    reloadAndSelect(id);
}

void LocationManagerDialog::onDeleteClicked()
{
    const int id = currentLocationId();
    if (id <= 0 || id == kRootLocationId)
        return;

    const Location current = LocationService::getLocationById(id);
    if (current.id <= 0)
        return;

    // Prevent deleting non-leaf nodes (schema uses ON DELETE RESTRICT for parent_id).
    const auto all = LocationService::getAllLocations();
    for (const auto& l : all) {
        if (l.parentId == id) {
            QMessageBox::warning(this, tr("Błąd"), tr("Nie można usunąć lokalizacji, która ma pod-lokalizacje."));
            return;
        }
    }

    const auto answer = QMessageBox::question(
        this,
        tr("Potwierdź usunięcie"),
        tr("Usunąć lokalizację: %1?\n\nProdukty i dokumenty mogą stracić przypisaną lokalizację (zostanie pusta).")
            .arg(current.path));

    if (answer != QMessageBox::Yes)
        return;

    const int parentIdToSelect = current.parentId;

    if (!LocationService::deleteLocation(id)) {
        QMessageBox::warning(this, tr("Błąd"), tr("Nie udało się usunąć lokalizacji."));
        return;
    }

    reloadAndSelect(parentIdToSelect > 0 ? parentIdToSelect : kRootLocationId);
}
