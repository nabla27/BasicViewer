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
#include "tablewidget.h"

class Graph2DSeries : public QWidget
{
    Q_OBJECT
public:
    explicit Graph2DSeries(TableWidget *table, QWidget *parent = nullptr);

public:
    void initializeData(const QList<QList<QList<float> > >& data);
    void updateGraph(QTableWidgetItem *changedItem);

private:
    QChart *graph;
    QVBoxLayout *vLayout;
    TableWidget *table;
    QList<QTableWidgetSelectionRange> selectedRanges;

    /* レイアウト */
    QGroupBox *labelGroup;

    /* 設定項目 */
    QList<QString> labelName;
};

#endif // GRAPH2DSERIES_H
