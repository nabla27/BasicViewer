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

    /* tableWidgetの選択範囲を設定 */
    setTableSelectedIndex();
    legendNameEdit.resize(plotTableRanges.size());

    /* グラフの初期化 */
    initializeGraph();

    /* windowのタイトルをファイル名に設定 */
    sheetName = table->getSheetName();
    setWindowTitle(sheetName);

    /* tableの変更に従い、グラフを再描画 */
    //ダブルクリック -> ファイルの保存 -> fileTreeのインデックス変更 -> sheet名の切り替え -> sheetの変更
    changedTableAction = connect(table, &TableWidget::itemChanged, [this](){
        if(this->table->getSheetName() == sheetName) updateGraphData();
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

void Graph2DSeries::setTableSelectedIndex()
{
    const QList<QTableWidgetSelectionRange> selectedRanges = table->selectedRanges();  //tableで選択されている範囲
    const qsizetype rangeCount = selectedRanges.size();                                //選択範囲の数
    qsizetype rangeIndex = 0;                                                          //どの選択範囲を指すかのインデックス

    for(;;)
    {
        const int startRow = selectedRanges.at(rangeIndex).topRow();
        const int startCol = selectedRanges.at(rangeIndex).leftColumn();
        const int endRow = selectedRanges.at(rangeIndex).bottomRow();
        const int endCol = selectedRanges.at(rangeIndex).rightColumn();

        if(endCol - startCol == 0){  //選択範囲の列幅が1のとき、プロットするxy座標の値は別々の選択範囲で指定されている
            if(rangeIndex + 1 >= rangeCount) { return; }
            plotTableRanges.append(plotTableRange(startRow, endRow, startCol, selectedRanges.at(rangeIndex + 1).leftColumn()));
            rangeIndex += 2;
        }
        else{  //選択範囲の列幅が2より大きいとき、プロットするxy座標の値は同じ選択範囲で連続した列で指定されている
            plotTableRanges.append(plotTableRange(startRow, endRow, startCol, startCol + 1));
            rangeIndex += 1;
        }

        if(rangeIndex >= rangeCount) { return; } //選択範囲が終了
    }
}

void Graph2DSeries::initializeGraph()
{
    /* データをセット */
    graph->removeAllSeries();   //データを初期化
    setGraphSeries();           //データのセット
    graph->createDefaultAxes(); //軸と格子の表示

    /* レイアウトの初期化 */
    initializeGraphLayout();
}

void Graph2DSeries::initializeGraphLayout()
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
        /* グラフのタイトル設定項目 */
        QGroupBox *titleGroup = new QGroupBox("Title", this);
        QVBoxLayout *titleBoxLayout = new QVBoxLayout(titleGroup);
        QLineEdit *titleEdit = new QLineEdit(titleGroup);
        vLayout->addWidget(titleGroup);
        titleGroup->setLayout(titleBoxLayout);
        titleBoxLayout->addWidget(titleEdit);
        connect(titleEdit, &QLineEdit::textEdited, graph, &QChart::setTitle);
    }
    {
        /* グラフのラベル設定項目 */
        QGroupBox *legendGroup = new QGroupBox("Legend name", this);
        legendBoxLayout = new QVBoxLayout;
        QCheckBox *checkBoxShowLegend = new QCheckBox("show legend", legendGroup); graph->legend()->setVisible(false);
        vLayout->addWidget(legendGroup);
        legendGroup->setLayout(legendBoxLayout);
        legendBoxLayout->addWidget(checkBoxShowLegend);
        connect(checkBoxShowLegend, &QCheckBox::toggled, this, &Graph2DSeries::changeLegendVisible);
        for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        {
            legendNameEdit[i]= new QLineEdit(legendGroup);
            legendBoxLayout->addWidget(legendNameEdit.at(i));
            connect(legendNameEdit.at(i), &QLineEdit::textEdited, graph->series().at(i), &QAbstractSeries::setName);
            legendNameEdit[i]->setVisible(false);
        }
    }
    {   //ラベルの設定項目
        QGroupBox *labelGroup = new QGroupBox("Label", this);
        QVBoxLayout *labelBoxLayout = new QVBoxLayout;
        QToolBox *labelToolBox = new QToolBox(labelGroup);
        vLayout->addWidget(labelGroup);
        labelGroup->setLayout(labelBoxLayout);
        labelBoxLayout->addWidget(labelToolBox);
        for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        {
            QWidget *toolBoxWidget = new QWidget(labelToolBox);
            QVBoxLayout *toolBoxLayout = new QVBoxLayout;
            QCheckBox *checkLabelVisible = new QCheckBox("Show label", toolBoxWidget);
            QCheckBox *checkLabelPointsVisible = new QCheckBox("Show label points", toolBoxWidget);
            labelToolBox->addItem(toolBoxWidget, "series" + QString::number(i));
            toolBoxWidget->setLayout(toolBoxLayout);
            toolBoxLayout->addWidget(checkLabelVisible);
            toolBoxLayout->addWidget(checkLabelPointsVisible);
            connect(checkLabelVisible, &QCheckBox::toggled, qobject_cast<QXYSeries*>(graph->series().at(i)), &QXYSeries::setPointsVisible);
            connect(checkLabelPointsVisible, &QCheckBox::toggled, qobject_cast<QXYSeries*>(graph->series().at(i)), &QXYSeries::setPointLabelsVisible);
            qobject_cast<QXYSeries*>(graph->series().at(i))->setPointLabelsClipping(false);
        }
    }
}

void Graph2DSeries::setGraphSeries()
{
    for(const plotTableRange& selectedRange : plotTableRanges)
    {
        QLineSeries *series = new QLineSeries;

        for(int row = selectedRange.startRow; row <= selectedRange.endRow; ++row)
        {
            if(table->item(row, selectedRange.colX) == nullptr || table->item(row, selectedRange.colY) == nullptr) continue;
            series->append(table->item(row, selectedRange.colX)->text().toFloat(),
                           table->item(row, selectedRange.colY)->text().toFloat());
        }

        graph->addSeries(series);
    }
}

void Graph2DSeries::updateGraphData()
{
    graph->removeAllSeries();

    setGraphSeries();

    graph->createDefaultAxes();
}

void Graph2DSeries::changeLegendVisible(bool visible)
{
    graph->legend()->setVisible(visible);
    for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        legendNameEdit[i]->setVisible(visible);
}

