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
#include <QToolBox>
#include <QLabel>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QComboBox>
#include <QSpacerItem>
#include <QValueAxis>
#include "tablewidget.h"

class Graph2DSeries : public QWidget
{
    Q_OBJECT
public:
    explicit Graph2DSeries(TableWidget *table, QWidget *parent = nullptr);
    ~Graph2DSeries();

private:
    void setTableSelectedIndex();
    void initializeGraph();
    void initializeGraphLayout();
    void setGraphSeries();
    void updateGraphData();

private:
    QChart *graph;
    //QVBoxLayout *vLayout;
    //QStackedWidget *settingWidget;
    TableWidget *table;
    QString sheetName;
    QMetaObject::Connection changedTableAction;

    struct plotTableRange
    {
        plotTableRange(int startRow, int endRow, int colX, int colY)
            : startRow(startRow), endRow(endRow), colX(colX), colY(colY) {}
        int startRow = 0;
        int endRow = 0;
        int colX = 0 ;
        int colY = 0;
    };
    QList<plotTableRange> plotTableRanges;

    /* レイアウト */
    QVBoxLayout *legendBoxLayout;
    QList<QLineEdit*> legendNameEdit;

private slots:
    void changeLegendVisible(bool visible);
};


#endif // GRAPH2DSERIES_H
