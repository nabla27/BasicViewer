#include "graphic3dbar.h"

Graphic3DBar::Graphic3DBar(QWidget *parent)
    : QWidget(parent)
{
    graph = new Q3DBars();
    container = QWidget::createWindowContainer(graph);

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    hLayout = new QHBoxLayout(this);
    hLayout->addWidget(container, 0);

    rowAxis = new QCategory3DAxis();
    colAxis = new QCategory3DAxis();
    rowAxis->setTitle("Row");
    colAxis->setTitle("Column");
    rowAxis->setTitleVisible(true);
    colAxis->setTitleVisible(true);
    graph->setRowAxis(rowAxis);
    graph->setColumnAxis(colAxis);

    setAttribute(Qt::WA_DeleteOnClose);
}

Graphic3DBar::~Graphic3DBar()
{
    delete rowAxis;
    delete colAxis;
    for(const QBar3DSeries* s : series) delete s;
    delete graph;
    delete container;
    delete hLayout;
}

void Graphic3DBar::setData(const QList<QList<QList<float>>> &list)
{
    int maxRowCount = 0;
    int maxColCount = 0;
    series.resize(list.size());

    for(qsizetype i = 0; i < list.size(); ++i)
    {
        QBarDataArray *array = new QBarDataArray;
        QBarDataRow *data;
        series[i] = new QBar3DSeries;
        const qsizetype listSize = list.at(i).size();
        array->reserve(listSize);

        for(qsizetype row = listSize - 1; row >= 0; --row)
        {
            data = new QBarDataRow(list.at(i).at(row).size());
            if(maxRowCount < row + 1) maxRowCount = row + 1;

            for(qsizetype col = 0; col < list.at(i).at(row).size(); ++col)
            {
                if(maxColCount < col + 1) maxColCount = col + 1;
                (*data)[col].setValue(list.at(i).at(row).at(col));
            }

            array->append(data);
        }

        series[i]->dataProxy()->resetArray(array);
        graph->addSeries(series.at(i));
    }

    QList<QString> rowLabel, colLabel;
    for(int i = maxRowCount; i >=1; --i)
        rowLabel << QString::number(i);
    for(int i = 1; i <= maxColCount; ++i)
        colLabel << QString::number(i);
    rowAxis->setLabels(rowLabel);
    colAxis->setLabels(colLabel);
}





























