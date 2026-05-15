#include "movementcarddelegate.h"

#include "../models/table/stockmovementslistmodel.h"

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>

MovementCardDelegate::MovementCardDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

static QRect insetRect(const QRect& r, int left, int top, int right, int bottom)
{
    return QRect(r.left() + left, r.top() + top, r.width() - left - right, r.height() - top - bottom);
}

void MovementCardDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!painter)
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const bool selected = (opt.state & QStyle::State_Selected);

    const QString typeName = index.data(StockMovementsListModel::TypeDisplayRole).toString();
    const QString occurredAt = index.data(StockMovementsListModel::OccurredAtDisplayRole).toString();
    const QString employeeName = index.data(StockMovementsListModel::EmployeeNameRole).toString();
    const QString fromLoc = index.data(StockMovementsListModel::FromLocationRole).toString();
    const QString toLoc = index.data(StockMovementsListModel::ToLocationRole).toString();
    const QString notes = index.data(StockMovementsListModel::NotesRole).toString();
    const bool canceled = index.data(StockMovementsListModel::CanceledRole).toBool();
    const int linesCount = index.data(StockMovementsListModel::LinesCountRole).toInt();

    // Card geometry
    const int outerMargin = 6;
    const QRect cardRect = insetRect(opt.rect, outerMargin, outerMargin / 2, outerMargin, outerMargin / 2);

    // Card background using palette (no hard-coded colors)
    QColor bg = opt.palette.color(QPalette::Base);
    QColor border = opt.palette.color(QPalette::Mid);
    QColor text = opt.palette.color(QPalette::Text);
    QColor subtleText = opt.palette.color(QPalette::PlaceholderText);

    if (selected) {
        bg = opt.palette.color(QPalette::Highlight);
        text = opt.palette.color(QPalette::HighlightedText);
        subtleText = text;
        border = bg;
    }

    QPainterPath path;
    const qreal radius = 8.0;
    path.addRoundedRect(cardRect, radius, radius);

    painter->fillPath(path, bg);
    painter->setPen(QPen(border, 1));
    painter->drawPath(path);

    // Content layout
    const int innerPad = 10;
    const QRect content = insetRect(cardRect, innerPad, innerPad, innerPad, innerPad);

    // Fonts
    QFont titleFont = opt.font;
    titleFont.setBold(true);

    QFont smallFont = opt.font;
    smallFont.setPointSizeF(opt.font.pointSizeF() > 0 ? opt.font.pointSizeF() - 1 : opt.font.pointSizeF());

    QFontMetrics titleFm(titleFont);
    QFontMetrics baseFm(opt.font);
    QFontMetrics smallFm(smallFont);

    const int line1H = titleFm.height();
    const int lineH = baseFm.height();

    QRect line1 = QRect(content.left(), content.top(), content.width(), line1H);
    QRect line2 = QRect(content.left(), line1.bottom() + 4, content.width(), lineH);
    QRect line3 = QRect(content.left(), line2.bottom() + 4, content.width(), lineH);

    // Line1: type (left) + date (right)
    painter->setFont(titleFont);
    painter->setPen(text);

    const QString typeElided = titleFm.elidedText(typeName, Qt::ElideRight, line1.width() * 2 / 3);
    painter->drawText(line1, Qt::AlignLeft | Qt::AlignVCenter, typeElided);

    painter->setFont(smallFont);
    painter->setPen(subtleText);
    const QString dateElided = smallFm.elidedText(occurredAt, Qt::ElideLeft, line1.width() / 3);
    painter->drawText(line1, Qt::AlignRight | Qt::AlignVCenter, dateElided);

    // Line2: employee / locations / count
    painter->setFont(opt.font);
    painter->setPen(text);

    QString middle;
    if (!employeeName.trimmed().isEmpty()) {
        middle = employeeName.trimmed();
    } else if (!fromLoc.trimmed().isEmpty() || !toLoc.trimmed().isEmpty()) {
        if (!fromLoc.trimmed().isEmpty() && !toLoc.trimmed().isEmpty())
            middle = QString("%1 → %2").arg(fromLoc.trimmed(), toLoc.trimmed());
        else if (!toLoc.trimmed().isEmpty())
            middle = toLoc.trimmed();
        else
            middle = fromLoc.trimmed();
    } else {
        middle = QString();
    }

    QString suffix;
    if (linesCount > 0)
        suffix = QString("Pozycji: %1").arg(linesCount);

    QString line2Text;
    if (!middle.isEmpty() && !suffix.isEmpty())
        line2Text = QString("%1 • %2").arg(middle, suffix);
    else
        line2Text = !middle.isEmpty() ? middle : suffix;

    painter->drawText(line2, Qt::AlignLeft | Qt::AlignVCenter, baseFm.elidedText(line2Text, Qt::ElideRight, line2.width()));

    // Line3: notes + canceled marker
    painter->setFont(smallFont);
    painter->setPen(subtleText);

    QString n = notes.trimmed();
    if (n.isEmpty())
        n = QString();

    if (canceled) {
        if (!n.isEmpty())
            n = QString("ANULOWANE • %1").arg(n);
        else
            n = "ANULOWANE";
    }

    painter->drawText(line3, Qt::AlignLeft | Qt::AlignVCenter, smallFm.elidedText(n, Qt::ElideRight, line3.width()));

    painter->restore();
}

QSize MovementCardDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex&) const
{
    QFont base = option.font;
    QFont title = base;
    title.setBold(true);

    QFont small = base;
    small.setPointSizeF(base.pointSizeF() > 0 ? base.pointSizeF() - 1 : base.pointSizeF());

    QFontMetrics titleFm(title);
    QFontMetrics baseFm(base);
    QFontMetrics smallFm(small);

    const int outerMargin = 6;
    const int innerPad = 10;
    const int spacing = 4;

    const int height =
        outerMargin +
        innerPad +
        titleFm.height() +
        spacing +
        baseFm.height() +
        spacing +
        smallFm.height() +
        innerPad +
        outerMargin;

    return QSize(option.rect.width(), height);
}
