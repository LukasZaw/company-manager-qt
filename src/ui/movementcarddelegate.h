#pragma once

#include <QStyledItemDelegate>

class MovementCardDelegate : public QStyledItemDelegate
{
public:
    explicit MovementCardDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
