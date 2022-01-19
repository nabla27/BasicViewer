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
 */

Graph2DSeries::Graph2DSeries(TableWidget *table, QWidget *parent)
    : QWidget(parent), table(table)
{
    graph = new QChart;
    /* レイアウトの右側設定部分 */
    vLayout = new QVBoxLayout;
    vLayout->setAlignment(Qt::AlignTop);

    /* レイアウトのグラフ部分 */
    QChartView *graphView = new QChartView(graph);
    graphView->setMinimumSize(640, 480);
    //graphView->setFrameShape(QFrame::Box);
    //graphView->setContentsMargins(0, 0, 0, 0);
    //graph->setContentsMargins(0, 0, 0, 0);
    //graph->setMargins(QMargins(0, 0, 0, 0));
    //graph->setWindowFrameMargins(0, 0, 0, 0);

    /* メインのレイアウト。各Widgetが水平方向に配列される */
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphView, Qt::AlignTop);
    mainLayout->addLayout(vLayout);
    setLayout(mainLayout);

    {//右側設定のレイアウト
        /* グラフのラベル設定項目 */
        labelGroup = new QGroupBox("Label name");
        vLayout->addWidget(labelGroup);
    }

    /* tableWidgetの選択された範囲を保存 */
    selectedRanges = table->selectedRanges();

    /* 選択されたデータを描写 */
    initializeData(table->selectedData<float>());

    /* windowのタイトルをsheetの名前を設定 */
    sheetName = table->getSheetName();
    setWindowTitle(sheetName);

    /* tableに変更があればグラフを再描画 */
    //ダブルクリック -> ファイルの保存 -> fileTreeのインデックス変更 -> sheet名の切り替え -> sheetの変更
    connect(table, &TableWidget::itemChanged, [this](){
        if(this->table->getSheetName() == sheetName)
            updateGraph();
    });

    /* ウィンドウが閉じられたら自動でdelete */
    setAttribute(Qt::WA_DeleteOnClose);
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
    labelName.resize(numData, "");  //ラベル名の初期設定

    /* 各レイアウトの設定 */
    {//ラベル設定
        QVBoxLayout *labelLayout = new QVBoxLayout;
        for(qsizetype i = 0; i < numData; ++i)
        {
            QLineEdit *labelNameEdit = new QLineEdit;
            labelLayout->addWidget(labelNameEdit);
            connect(labelNameEdit, &QLineEdit::editingFinished, [i, labelNameEdit, this](){
                labelName[i] = labelNameEdit->text();
                updateGraph(nullptr);
            });
        }
        labelGroup->setLayout(labelLayout);
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
        series->setName(labelName.at(index));

        for(int row = startRow; row <= endRow; ++row)
        {
            if(table->item(row, startCol) == nullptr || table->item(row, startCol + 1) == nullptr) continue;
            series->append(table->item(row, startCol)->text().toFloat(),
                           table->item(row, startCol + 1)->text().toFloat());
        }

        graph->addSeries(series);
        index++;
    }

    graph->createDefaultAxes();
}
