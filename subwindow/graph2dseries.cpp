#include "graph2dseries.h"

/*
 *
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * window* * * * * * * * * * * * * * * * * * * * *
 *                                                                                       *
 *   * * * * * * * * * * * * * * * * * * mainLayout* * * * * * * * * * * * * * * * * *   *
 *   *                                                                               *   *
 *   *   * * * * * * * * GraphView * * * * * * * * * * * * *scrollArea * * * * * *   *   *
 *   *   *                                       * * * * * areaContents* * * * * *   *   *
 *   *   *                                       * * * * * * vLayout * * * * * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *          Combo          * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *       StackWidget       * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * *                         * * *   *   *
 *   *   *                                       * * * * * * * * * * * * * * * * *   *   *
 *   *   *                                       * * * * * * * * * * * * * * * * *   *   *
 *   *   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *   *   *
 *   *                                                                               *   *
 *   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *   *
 *                                                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *
 *
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
    disconnect(changedTableAction);
}

void Graph2DSeries::setTableSelectedIndex()
{    
    const QList<QTableWidgetSelectionRange> selectedRanges = table->selectedRanges();  //tableで選択されている範囲
    const qsizetype rangeCount = selectedRanges.size();                                //選択範囲の数
    qsizetype rangeIndex = 0;                                                          //どの選択範囲を指すかのインデックス

    if(rangeCount == 0) { return; }                                                    //選択範囲がなければ無効

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

    /* グラフの初期設定 */
    graph->legend()->setVisible(false);

    /* レイアウトの初期化 */
    initializeGraphLayout();
}

void Graph2DSeries::initializeGraphLayout()
{
    /* レイアウトのグラフ部分 */
    QChartView *graphView = new QChartView(graph);
    graphView->setMinimumSize(320, 240);

    /* レイアウトの右側設定部分 */
    QScrollArea *settingScrollArea = new QScrollArea(this);
    QWidget *settingScrollAreaContents = new QWidget(settingScrollArea);
    QVBoxLayout *vLayout = new QVBoxLayout(settingScrollAreaContents);
    QComboBox *selectPageCombo = new QComboBox(settingScrollAreaContents);
    QStackedWidget *settingStackWidget = new QStackedWidget(settingScrollAreaContents);
    settingScrollArea->setWidget(settingScrollAreaContents);
    settingScrollArea->setWidgetResizable(true);
    settingScrollAreaContents->setLayout(vLayout);
    vLayout->addWidget(selectPageCombo);
    vLayout->addWidget(settingStackWidget);

    /* メインのレイアウト。各Widget(vLayout,graphView)が水平方向に配列される */
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphView, Qt::AlignTop);
    mainLayout->addWidget(settingScrollArea);
    setLayout(mainLayout);

    /* 右側の設定部分のレイアウト */
    {
        selectPageCombo->addItem("Title");
        selectPageCombo->addItem("Legend");
        selectPageCombo->addItem("Label");
        selectPageCombo->addItem("Range");
        connect(selectPageCombo, &QComboBox::activated, settingStackWidget, &QStackedWidget::setCurrentIndex);
    }
    {
        /* グラフタイトルの設定項目 */
        QWidget *titleSettingWidget = new QWidget(this);
        QVBoxLayout *titleSettingLayout = new QVBoxLayout(titleSettingWidget);
        QLineEdit *titleEdit = new QLineEdit(titleSettingWidget);
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        settingStackWidget->addWidget(titleSettingWidget);
        titleSettingWidget->setLayout(titleSettingLayout);
        titleSettingLayout->addWidget(titleEdit, Qt::AlignTop);
        titleSettingLayout->addItem(verticalSpacer);
        connect(titleEdit, &QLineEdit::textEdited, graph, &QChart::setTitle);
    }
    {
        /* グラフ凡例の設定項目 */
        QWidget *legendSettingWidget = new QWidget(this);
        QVBoxLayout *legendSettingLayout = new QVBoxLayout(legendSettingWidget);
        QCheckBox *checkBoxShowLegend = new QCheckBox("show legend", legendSettingWidget);
        QLabel *legendTextEditLabel = new QLabel("・text", legendSettingWidget);
        settingStackWidget->addWidget(legendSettingWidget);
        legendSettingWidget->setLayout(legendSettingLayout);
        legendSettingLayout->addWidget(checkBoxShowLegend);
        legendSettingLayout->addWidget(legendTextEditLabel);
        connect(checkBoxShowLegend, &QCheckBox::toggled, this, &Graph2DSeries::changeLegendVisible);
        connect(checkBoxShowLegend, &QCheckBox::toggled, legendTextEditLabel, &QLabel::setVisible);
        for(qsizetype i = 0; i < plotTableRanges.size(); ++i){
            legendNameEdit[i] = new QLineEdit(legendSettingWidget);
            legendSettingLayout->addWidget(legendNameEdit.at(i));
            connect(legendNameEdit.at(i), &QLineEdit::textEdited, graph->series().at(i), &QAbstractSeries::setName);
            legendNameEdit.at(i)->setVisible(false);
        }
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        legendSettingLayout->addItem(verticalSpacer);
    }
    {
        /* グラフラベルの設定項目 */
        QWidget *labelSettingWidget = new QWidget(this);
        QVBoxLayout *labelSettingLayout = new QVBoxLayout(labelSettingWidget);
        QToolBox *labelToolBox = new QToolBox(labelSettingWidget);
        settingStackWidget->addWidget(labelSettingWidget);
        labelSettingWidget->setLayout(labelSettingLayout);
        labelSettingLayout->addWidget(labelToolBox);
        for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        {
            QWidget *toolBoxWidget = new QWidget(labelToolBox);
            QVBoxLayout *toolBoxLayout = new QVBoxLayout(toolBoxWidget);
            QCheckBox *checkLabelVisible = new QCheckBox("Show label", toolBoxWidget);
            QCheckBox *checkLabelPointsVisible = new QCheckBox("Show label points", toolBoxWidget);
            QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
            labelToolBox->addItem(toolBoxWidget, "series " + QString::number(i));
            toolBoxWidget->setLayout(toolBoxLayout);
            toolBoxLayout->addWidget(checkLabelVisible);
            toolBoxLayout->addWidget(checkLabelPointsVisible);
            toolBoxLayout->addItem(verticalSpacer);
            connect(checkLabelVisible, &QCheckBox::toggled, qobject_cast<QXYSeries*>(graph->series().at(i)), &QXYSeries::setPointsVisible);
            connect(checkLabelPointsVisible, &QCheckBox::toggled, qobject_cast<QXYSeries*>(graph->series().at(i)), &QXYSeries::setPointLabelsVisible);
            qobject_cast<QXYSeries*>(graph->series().at(i))->setPointLabelsClipping(false);
        }
    }
    {
        /* グラフ軸の設定項目 */
        QWidget *axisSettingWidget = new QWidget(this);
        QVBoxLayout *axisSettingLayout = new QVBoxLayout(axisSettingWidget);
        QGroupBox *xAxisGroupBox = new QGroupBox("X Axis", axisSettingWidget);
        QGroupBox *yAxisGroupBox = new QGroupBox("Y Axis", axisSettingWidget);
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        settingStackWidget->addWidget(axisSettingWidget);
        axisSettingWidget->setLayout(axisSettingLayout);
        axisSettingLayout->addWidget(xAxisGroupBox);
        axisSettingLayout->addWidget(yAxisGroupBox);
        axisSettingLayout->addItem(verticalSpacer);
        /* x軸 */
        const int labelWidth = 54;
        QVBoxLayout *xAxisLayout = new QVBoxLayout(xAxisGroupBox);
        QHBoxLayout *rangeMinXLayout = new QHBoxLayout();
        QLabel *minXLabel = new QLabel("Min", xAxisGroupBox);
        QLineEdit *minXEdit = new QLineEdit(xAxisGroupBox);
        QSpacerItem *horizontalMinXSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QHBoxLayout *rangeMaxXLayout = new QHBoxLayout();
        QLabel *maxXLabel = new QLabel("Max", xAxisGroupBox); maxXLabel->setFrameShadow(QFrame::Plain);
        QLineEdit *maxXEdit = new QLineEdit(xAxisGroupBox);
        QSpacerItem *horizontalMaxXSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QHBoxLayout *xAxisNameLayout = new QHBoxLayout();
        QLabel *xAxisNameLabel = new QLabel("Axis name", yAxisGroupBox);
        QLineEdit *xAxisNameEdit = new QLineEdit(xAxisGroupBox);
        QCheckBox *checkShowAxisHorizontalGrid = new QCheckBox("Show horizontal grid", xAxisGroupBox);
        xAxisGroupBox->setLayout(xAxisLayout);
        xAxisLayout->addLayout(rangeMinXLayout);
        rangeMinXLayout->addWidget(minXLabel);
        rangeMinXLayout->addWidget(minXEdit);
        rangeMinXLayout->addItem(horizontalMinXSpacer);
        xAxisLayout->addLayout(rangeMaxXLayout);
        rangeMaxXLayout->addWidget(maxXLabel);
        rangeMaxXLayout->addWidget(maxXEdit);
        rangeMaxXLayout->addItem(horizontalMaxXSpacer);
        xAxisLayout->addLayout(xAxisNameLayout);
        xAxisNameLayout->addWidget(xAxisNameLabel);
        xAxisNameLayout->addWidget(xAxisNameEdit);
        xAxisLayout->addWidget(checkShowAxisHorizontalGrid);
        minXLabel->setMinimumWidth(labelWidth);
        minXEdit->setMaximumWidth(30);
        maxXLabel->setMinimumWidth(labelWidth);
        maxXEdit->setMaximumWidth(30);
        checkShowAxisHorizontalGrid->setChecked(true);
        if(plotTableRanges.size() > 0){
            minXEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast())->min()));
            maxXEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast())->max()));
            connect(minXEdit, &QLineEdit::textEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setMin);
            connect(maxXEdit, &QLineEdit::textEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setMax);
            connect(xAxisNameEdit, &QLineEdit::textEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setTitleText);
            connect(checkShowAxisHorizontalGrid, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setGridLineVisible);
        }
        /* y軸 */
        QVBoxLayout *yAxisLayout = new QVBoxLayout(yAxisGroupBox);
        QHBoxLayout *rangeMinYLayout = new QHBoxLayout();
        QLabel *minYLabel = new QLabel("Min", yAxisGroupBox);
        QLineEdit *minYEdit = new QLineEdit(yAxisGroupBox);
        QSpacerItem *horizontalMinYSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QHBoxLayout *rangeMaxYLayout = new QHBoxLayout();
        QLabel *maxYLabel = new QLabel("Max", yAxisGroupBox);
        QLineEdit *maxYEdit = new QLineEdit(yAxisGroupBox);
        QSpacerItem *horizontalMaxYSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QHBoxLayout *yAxisNameLayout = new QHBoxLayout();
        QLabel *yAxisNameLabel = new QLabel("Axis name", yAxisGroupBox);
        QLineEdit *yAxisNameEdit = new QLineEdit(yAxisGroupBox);
        QCheckBox *checkShowAxisVerticalGrid = new QCheckBox("Show vertical grid", xAxisGroupBox);
        yAxisGroupBox->setLayout(yAxisLayout);
        yAxisLayout->addLayout(rangeMinYLayout);
        rangeMinYLayout->addWidget(minYLabel);
        rangeMinYLayout->addWidget(minYEdit);
        rangeMinYLayout->addItem(horizontalMinYSpacer);
        yAxisLayout->addLayout(rangeMaxYLayout);
        rangeMaxYLayout->addWidget(maxYLabel);
        rangeMaxYLayout->addWidget(maxYEdit);
        rangeMaxYLayout->addItem(horizontalMaxYSpacer);
        yAxisLayout->addLayout(yAxisNameLayout);
        yAxisNameLayout->addWidget(yAxisNameLabel);
        yAxisNameLayout->addWidget(yAxisNameEdit);
        minYLabel->setMinimumWidth(labelWidth);
        yAxisLayout->addWidget(checkShowAxisVerticalGrid);
        minYEdit->setMaximumWidth(30);
        maxYLabel->setMinimumWidth(labelWidth);
        maxYEdit->setMaximumWidth(30);
        checkShowAxisVerticalGrid->setChecked(true);
        if(plotTableRanges.size() > 0){
            minYEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast())->min()));
            maxYEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast())->max()));
            connect(minYEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setMin);
            connect(maxYEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setMax);
            connect(yAxisNameEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setTitleText);
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setGridLineVisible);
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

