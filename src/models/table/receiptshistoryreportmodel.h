#ifndef RECEIPTSHISTORYREPORTMODEL_H
#define RECEIPTSHISTORYREPORTMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QVector>

class ReceiptsHistoryReportModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct Row {
        int movementId{0};
        QDateTime occurredAt;
        QString employee;
        QString sku;
        QString product;
        double quantity{0.0};
        QString unit;
        QString toLocation;
        QString notes;
        bool canceled{false};
    };

    enum Column {
        OccurredAt = 0,
        MovementId,
        Employee,
        Sku,
        Product,
        Quantity,
        Unit,
        ToLocation,
        Notes,
        Canceled,
        ColumnCount
    };

    explicit ReceiptsHistoryReportModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void reload();
    QVector<Row> rows() const;

private:
    QVector<Row> m_rows;
};

#endif // RECEIPTSHISTORYREPORTMODEL_H
