#include "serialplot1d.h"

SerialPlot1D::SerialPlot1D(QWidget *parent)
    : QWidget(parent)
{
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.3f, 0.4f));

    graph = new QChart;
    graphView = new QChartView(graph);
    series = new QLineSeries;
    graph->addSeries(series);
    axisX = new QValueAxis;
    axisY = new QValueAxis;
    graph->addAxis(axisX, Qt::Alignment(Qt::AlignmentFlag::AlignBottom));
    graph->addAxis(axisY, Qt::Alignment(Qt::AlignmentFlag::AlignLeft));
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    axisX->setTitleText("X");
    axisY->setTitleText("Y");


    QHBoxLayout *layout = new QHBoxLayout(this);
    setLayout(layout);
    layout->addWidget(graphView);
}

SerialPlot1D::~SerialPlot1D()
{
    delete graph;
    delete series;
    delete axisX;
    delete axisY;
}

void SerialPlot1D::initializeLayout()
{

}
