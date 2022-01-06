#include "graphic3dscatter.h"

Graphic3DScatter::Graphic3DScatter(QWidget *parent) : QWidget(parent)
{
    graph = new Q3DScatter();
    container = QWidget::createWindowContainer(graph);

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    hLayout = new QHBoxLayout(this);
    hLayout->addWidget(container, 0);

    /* 軸の初期化 */
    xAxis = new QValue3DAxis;
    yAxis = new QValue3DAxis;
    zAxis = new QValue3DAxis;
    xAxis->setTitle("X"); xAxis->setTitleVisible(true);
    yAxis->setTitle("Z"); yAxis->setTitleVisible(true);  //z軸とy軸を入れ替えるz=x×yとなるように
    zAxis->setTitle("Y"); zAxis->setTitleVisible(true);
    graph->setAxisX(xAxis);
    graph->setAxisY(yAxis);
    graph->setAxisZ(zAxis);

    /* 影を無効にする */
    graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

    /* ウィンドウを閉じたら自動でデストラクタを呼ぶ */
    setAttribute(Qt::WA_DeleteOnClose);
}

Graphic3DScatter::~Graphic3DScatter()
{
    for(const QScatter3DSeries *s : series) delete s;
    delete xAxis;
    delete yAxis;
    delete zAxis;
    delete graph;
    delete container;
    delete hLayout;
}

void Graphic3DScatter::setData(const QList<QList<QList<float>>> &list)
{
    series.resize(list.size());

    for(qsizetype i = 0; i < list.size(); ++i)
    {
        series[i] = new QScatter3DSeries;
        series[i]->setMesh(QAbstract3DSeries::Mesh(i % 7 + 2));
        QScatterDataArray data;

        for(qsizetype j = 0; j < list.at(i).size(); ++j)
        {
            if(list.at(i).at(j).size() < 3) { continue; }
            data << QVector3D(list.at(i).at(j).at(0),
                              list.at(i).at(j).at(2),
                              list.at(i).at(j).at(1));
        }

        series[i]->dataProxy()->addItems(data);
        graph->addSeries(series.at(i));
    }
}



















