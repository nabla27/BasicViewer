#include "graph2dseries.h"

/* * * * * * * * * * * * * * * * window* * * * * * * * * * * * * * * * *
 *                                                                     *
 *   * * * * * * * * * * * * * mainLayout* * * * * * * * * * * * * *   *
 *   *                                                             *   *
 *   *   * * * * * * * * GraphView * * * * * * * * * *vLayout* *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   *                                       * *           *   *   *
 *   *   * * * * * * * * * * * * * * * * * * * * * * * * * * * *   *   *
 *   *                                                             *   *
 *   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *   *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 * this ---|--- mainLayout
 *         |--- legendGroup
 *         |--- labelGroup
 */


Graph2DSeries::Graph2DSeries(TableWidget *table, QWidget *parent)
    : QWidget(parent), table(table)
{
    graph = new QChart;

    /* tableWidgetの選択された範囲を保存 */
    selectedRanges = table->selectedRanges();

    /* レイアウトの初期化 */
    initializeLayout();

    /* 選択されたデータで初期化,描写 */
    initializeData(table->selectedData<float>());

    /* windowのタイトルをsheetの名前を設定 */
    sheetName = table->getSheetName();
    setWindowTitle(sheetName);

    /* tableに変更があればグラフを再描画 */
    //ダブルクリック -> ファイルの保存 -> fileTreeのインデックス変更 -> sheet名の切り替え -> sheetの変更
    changedTableAction =  connect(table, &TableWidget::itemChanged, [this](){
        if(this->table->getSheetName() == sheetName) updateGraph();
    });

    /* ウィンドウが閉じられたら自動でdelete */
    setAttribute(Qt::WA_DeleteOnClose);
}

Graph2DSeries::~Graph2DSeries()
{
    delete graph;
    delete vLayout;
    delete legendBoxLayout;
    disconnect(changedTableAction);
}


void Graph2DSeries::initializeData(const QList<QList<QList<float>>> &data)
{
    graph->removeAllSeries();
    const qsizetype numData = data.size();

    for(const QList<QList<float> >& list : data)
    {
        QLineSeries *series = new QLineSeries;

        for(const QList<float>& coord : list)
        {
            if(coord.size() < 2) { continue; }
            series->append(coord.at(0), coord.at(1));
        }

        graph->addSeries(series);
    }

    graph->createDefaultAxes(); //軸と格子の表示

    /* 各初期設定 */
    legendName.resize(numData, "");  //ラベル名の初期設定

    /* 各レイアウトの設定 */
    {//ラベル設定
        for(qsizetype i = 0; i < numData; ++i)
        {
            QLineEdit *labelNameEdit = new QLineEdit;
            legendBoxLayout->addWidget(labelNameEdit);
            connect(labelNameEdit, &QLineEdit::editingFinished, [i, labelNameEdit, this](){
                legendName[i] = labelNameEdit->text();
                updateGraph(nullptr);
            });
        }
    }
}

void Graph2DSeries::initializeLayout()
{
    /* レイアウトのグラフ部分 */
    QChartView *graphView = new QChartView(graph);
    graphView->setMinimumSize(320, 240);

    /* レイアウトの右側設定部分 */
    vLayout = new QVBoxLayout;
    vLayout->setAlignment(Qt::AlignTop);

    /* メインのレイアウト。各Widget(vLayout,graphView)が水平方向に配列される */
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphView, Qt::AlignTop);
    mainLayout->addLayout(vLayout);
    setLayout(mainLayout);

    /* 右側の設定部分のレイアウト */
    {
        /* グラフのラベル設定項目 */
        legendBoxLayout = new QVBoxLayout;
        QGroupBox *legendGroup = new QGroupBox("Label name", this);
        legendGroup->setLayout(legendBoxLayout);
        vLayout->addWidget(legendGroup);
        QCheckBox *checkBoxShowLegend = new QCheckBox("show legend", legendGroup);
        checkBoxShowLegend->setChecked(true);
        legendBoxLayout->addWidget(checkBoxShowLegend);
        connect(checkBoxShowLegend, &QCheckBox::toggled, [this, checkBoxShowLegend](){
             isVisibleLegend = checkBoxShowLegend->isChecked();
             updateGraph();
        });
    }
    {
        /* ポイントを表示するか */
        QGroupBox *labelGroup = new QGroupBox("Label", this);
        QVBoxLayout *labelBoxLayout = new QVBoxLayout(labelGroup);
        labelGroup->setLayout(labelBoxLayout);
        vLayout->addWidget(labelGroup);
        QCheckBox *checkBoxShowLabel = new QCheckBox("show label", labelGroup);
        labelBoxLayout->addWidget(checkBoxShowLabel);
        connect(checkBoxShowLabel, &QCheckBox::toggled, [this, checkBoxShowLabel](){
            isVisibleLabel = checkBoxShowLabel->isChecked();
            updateGraph();
        });
        QCheckBox *checkBoxShowLabelPoints = new QCheckBox("Show label points", labelGroup);
        labelBoxLayout->addWidget(checkBoxShowLabelPoints);
        connect(checkBoxShowLabelPoints, &QCheckBox::toggled, [this, checkBoxShowLabelPoints](){
            isVisibleLabelPoints = checkBoxShowLabelPoints->isChecked();
            updateGraph();
        });
    }
}

void Graph2DSeries::updateGraph(QTableWidgetItem*)
{
    graph->removeAllSeries();
    qsizetype index = 0;

    for(const QTableWidgetSelectionRange& selected : selectedRanges)
    {
        const int startRow = selected.topRow();
        const int startCol = selected.leftColumn();
        const int endRow = selected.bottomRow();

        QLineSeries *series = new QLineSeries;
        series->setName(legendName.at(index));

        for(int row = startRow; row <= endRow; ++row)
        {
            if(table->item(row, startCol) == nullptr || table->item(row, startCol + 1) == nullptr) continue;
            series->append(table->item(row, startCol)->text().toFloat(),
                           table->item(row, startCol + 1)->text().toFloat());
        }

        graph->legend()->setVisible(isVisibleLegend);

        series->setPointsVisible(isVisibleLabel);
        series->setPointLabelsVisible(isVisibleLabelPoints);
        series->setPointLabelsClipping(false);

        graph->addSeries(series);
        index++;
    }

    graph->createDefaultAxes();
}
