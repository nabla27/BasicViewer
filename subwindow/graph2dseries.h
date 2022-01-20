#ifndef GRAPH2DSERIES_H
#define GRAPH2DSERIES_H
#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineSeries>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include "tablewidget.h"

class Graph2DSeries : public QWidget
{
    Q_OBJECT
public:
    explicit Graph2DSeries(TableWidget *table, QWidget *parent = nullptr);
    ~Graph2DSeries();

public:
    void initializeData(const QList<QList<QList<float> > >& data);
    void updateGraph(QTableWidgetItem *changedItem = nullptr);

private:
    QChart *graph;
    QVBoxLayout *vLayout;
    TableWidget *table;
    QList<QTableWidgetSelectionRange> selectedRanges;
    QString sheetName;
    QMetaObject::Connection changedTableAction;

    /* レイアウト */
    QGroupBox *legendGroup;

    /* 設定項目 */
    QList<QString> legendName;
    bool isVisibleLabel = false;
    bool isVisibleLabelPoints = false;
};

#endif // GRAPH2DSERIES_H
