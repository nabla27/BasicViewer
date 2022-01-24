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
    legendName.resize(plotTableRanges.size(), "");

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
    /* レイアウトの初期化 */
    initializeGraphLayout();

    /* データをセット */
    graph->removeAllSeries();   //データを初期化
    setGraphSeries();           //データのセット
    graph->createDefaultAxes(); //軸と格子の表示
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
        /* グラフのラベル設定項目 */
        legendBoxLayout = new QVBoxLayout;
        for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        {
            QLineEdit *labelNameEdit = new QLineEdit;
            legendBoxLayout->addWidget(labelNameEdit);
            connect(labelNameEdit, &QLineEdit::editingFinished, [i, labelNameEdit, this](){
                legendName[i] = labelNameEdit->text();
                updateGraphLayout();
            });
        }
        QGroupBox *legendGroup = new QGroupBox("Label name", this);
        legendGroup->setLayout(legendBoxLayout);
        vLayout->addWidget(legendGroup);
        QCheckBox *checkBoxShowLegend = new QCheckBox("show legend", legendGroup);
        checkBoxShowLegend->setChecked(true);
        legendBoxLayout->addWidget(checkBoxShowLegend);
        connect(checkBoxShowLegend, &QCheckBox::toggled, [this, checkBoxShowLegend](){
             isVisibleLegend = checkBoxShowLegend->isChecked();
             updateGraphLayout();
        });
    }
    {
        /* グラフのポイント表示設定項目 */
        QGroupBox *labelGroup = new QGroupBox("Label", this);
        QVBoxLayout *labelBoxLayout = new QVBoxLayout(labelGroup);
        labelGroup->setLayout(labelBoxLayout);
        vLayout->addWidget(labelGroup);
        QCheckBox *checkBoxShowLabel = new QCheckBox("show label", labelGroup);
        labelBoxLayout->addWidget(checkBoxShowLabel);
        connect(checkBoxShowLabel, &QCheckBox::toggled, [this, checkBoxShowLabel](){
            isVisibleLabel = checkBoxShowLabel->isChecked();
            updateGraphLayout();
        });
        QCheckBox *checkBoxShowLabelPoints = new QCheckBox("Show label points", labelGroup);
        labelBoxLayout->addWidget(checkBoxShowLabelPoints);
        connect(checkBoxShowLabelPoints, &QCheckBox::toggled, [this, checkBoxShowLabelPoints](){
            isVisibleLabelPoints = checkBoxShowLabelPoints->isChecked();
            updateGraphLayout();
        });
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

void Graph2DSeries::updateGraphLayout()
{
    /* 凡例の表示有無 */
    graph->legend()->setVisible(isVisibleLegend);

    qsizetype index = 0;

    for(QAbstractSeries *series : graph->series())
    {
        /* 凡例のテキスト設定 */
        series->setName(legendName.at(index));

        qobject_cast<QXYSeries*>(series)->setPointsVisible(isVisibleLabel);            //ラベル(点)の表示の有無
        qobject_cast<QXYSeries*>(series)->setPointLabelsVisible(isVisibleLabelPoints); //ラベル(点)の座標表示の有無
        qobject_cast<QXYSeries*>(series)->setPointLabelsClipping(false);

        index++;
    }
}

