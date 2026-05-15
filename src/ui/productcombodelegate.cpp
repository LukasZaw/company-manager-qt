#include "productcombodelegate.h"

#include "../models/table/stockmovementlinesmodel.h"

#include <QComboBox>
#include <QCompleter>

ProductComboDelegate::ProductComboDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void ProductComboDelegate::setProducts(const QList<ProductItem>& products)
{
    m_products = products;
}

QWidget* ProductComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    auto* combo = new QComboBox(parent);
    combo->setEditable(true);
    combo->setInsertPolicy(QComboBox::NoInsert);

    combo->addItem(QString(), 0);
    for (const auto& p : m_products) {
        const QString label = QString("%1 — %2").arg(p.sku.trimmed(), p.name.trimmed());
        combo->addItem(label, p.id);
    }

    auto* completer = new QCompleter(combo);
    completer->setModel(combo->model());
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    combo->setCompleter(completer);

    return combo;
}

void ProductComboDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto* combo = qobject_cast<QComboBox*>(editor);
    if (!combo)
        return;

    const int currentId = index.data(StockMovementLinesModel::ProductIdRole).toInt();
    int found = -1;
    for (int i = 0; i < combo->count(); ++i) {
        if (combo->itemData(i).toInt() == currentId) {
            found = i;
            break;
        }
    }

    if (found >= 0)
        combo->setCurrentIndex(found);
    else
        combo->setCurrentIndex(0);
}

void ProductComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto* combo = qobject_cast<QComboBox*>(editor);
    if (!combo || !model)
        return;

    const int productId = combo->currentData().toInt();
    model->setData(index, productId, Qt::EditRole);
}

void ProductComboDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
{
    if (!editor)
        return;

    editor->setGeometry(option.rect);
}
