#ifndef SERIALPLOT1D_H
#define SERIALPLOT1D_H
#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QLayout>
#include "utility.h"

class SerialPlot1D : public QWidget
{
    Q_OBJECT
public:
    explicit SerialPlot1D(QWidget *parent = nullptr);
    ~SerialPlot1D();

    void initializeLayout();

private:
    QChart *graph;
    QChartView *graphView;
    QXYSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
    qsizetype dataCount = 0;

signals:

};

#endif // SERIALPLOT1D_H
