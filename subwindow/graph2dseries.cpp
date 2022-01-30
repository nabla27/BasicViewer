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
    setGeometry(0, 0, 620, 240);

    /* レイアウトのグラフ部分 */
    graphView = new QChartView(graph);   //graphViewのウィンドウサイズは4:3がいい感じ

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

    /* メインのレイアウト。各Widget(graphView, settingScrollArea)が水平方向に配列される */
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphView, Qt::AlignTop);
    mainLayout->addWidget(settingScrollArea);
    setLayout(mainLayout);

    /* 右側の設定部分のレイアウト */
#define LabelWidth 80
#define EditWidth1 35
    {
        selectPageCombo->addItem("Graph");
        selectPageCombo->addItem("Legend");
        selectPageCombo->addItem("Label");
        selectPageCombo->addItem("Axis");
        selectPageCombo->addItem("Export");
        connect(selectPageCombo, &QComboBox::activated, settingStackWidget, &QStackedWidget::setCurrentIndex);
    }
    {
        /* グラフの設定項目 */
        QWidget *graphSettingWidget = new QWidget(settingStackWidget);
        QVBoxLayout *graphSettingLayout = new QVBoxLayout(graphSettingWidget);

        QHBoxLayout *titleEditLayout = new QHBoxLayout();
        QLabel *titleEditLabel = new QLabel("Title", graphSettingWidget);
        QLineEdit *titleEdit = new QLineEdit(graphSettingWidget);
        QSpacerItem *horizontalTitleEditSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *themeSetLayout = new QHBoxLayout();
        QLabel *themeSetLabel = new QLabel("Theme", graphSettingWidget);
        QComboBox *themeSetCombo = new QComboBox(graphSettingWidget);
        QSpacerItem *horizontalThemeSetSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        settingStackWidget->addWidget(graphSettingWidget);
        graphSettingWidget->setLayout(graphSettingLayout);

        graphSettingLayout->addLayout(titleEditLayout);
        titleEditLayout->addWidget(titleEditLabel);
        titleEditLayout->addWidget(titleEdit);
        titleEditLayout->addItem(horizontalTitleEditSpacer);

        graphSettingLayout->addLayout(themeSetLayout);
        themeSetLayout->addWidget(themeSetLabel);
        themeSetLayout->addWidget(themeSetCombo);
        themeSetLayout->addItem(horizontalThemeSetSpacer);

        graphSettingLayout->addItem(verticalSpacer);

        titleEditLabel->setMinimumWidth(LabelWidth);
        themeSetLabel->setMinimumWidth(LabelWidth);
        themeSetCombo->insertItems(0, themeNameList);

        connect(titleEdit, &QLineEdit::textEdited, graph, &QChart::setTitle);
        connect(themeSetCombo, &QComboBox::currentIndexChanged, [this, themeSetCombo](){
            graph->setTheme(QChart::ChartTheme(themeSetCombo->currentIndex()));
        });
    }
    {
        /* グラフ凡例の設定項目 */
        QWidget *legendSettingWidget = new QWidget(settingStackWidget);
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
        QWidget *labelSettingWidget = new QWidget(settingStackWidget);
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
        QWidget *axisSettingWidget = new QWidget(settingStackWidget);
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
        QVBoxLayout *xAxisLayout = new QVBoxLayout(xAxisGroupBox);

        QHBoxLayout *rangeMinXLayout = new QHBoxLayout();
        QLabel *minXLabel = new QLabel("Min", xAxisGroupBox);
        QLineEdit *minXEdit = new QLineEdit(xAxisGroupBox);
        QSpacerItem *horizontalMinXSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *rangeMaxXLayout = new QHBoxLayout();
        QLabel *maxXLabel = new QLabel("Max", xAxisGroupBox); maxXLabel->setFrameShadow(QFrame::Plain);
        QLineEdit *maxXEdit = new QLineEdit(xAxisGroupBox);
        QSpacerItem *horizontalMaxXSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QCheckBox *checkXAxisNameVisible = new QCheckBox("Show axis name", xAxisGroupBox);

        QHBoxLayout *xAxisNameLayout = new QHBoxLayout();
        QLabel *xAxisNameLabel = new QLabel("Axis name", yAxisGroupBox);
        QLineEdit *xAxisNameEdit = new QLineEdit(xAxisGroupBox);
        QSpacerItem *xAxisNameSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *xAxisNameSizeLayout = new QHBoxLayout();
        QLabel *xAxisNameSizeLabel = new QLabel("Axis name size", yAxisGroupBox);
        QSpinBox *xAxisNameSizeSpin = new QSpinBox(yAxisGroupBox);
        QSpacerItem *xAxisNameSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QCheckBox *checkHorizontalLabelVisible = new QCheckBox("Show label", yAxisGroupBox);

        QHBoxLayout *horizontalLabelAngleLayout = new QHBoxLayout();
        QLabel *horizontalLabelAngleLabel = new QLabel("Label angle", xAxisGroupBox);
        QLineEdit *horizontalLabelAngleEdit = new QLineEdit(xAxisGroupBox);
        QSpacerItem *horizontalLabelAngleSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *horizontalLabelColorLayout = new QHBoxLayout();
        QLabel *horizontalLabelColorLabel = new QLabel("Label color", xAxisGroupBox);
        QComboBox *horizontalLabelColorCombo = new QComboBox(xAxisGroupBox);
        QSpacerItem *horizontalLabelColorSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *horizontalLabelSizeLayout = new QHBoxLayout();
        QLabel *horizontalLabelSizeLabel = new QLabel("Label size", xAxisGroupBox);
        QSpinBox *horizontalLabelSizeSpin = new QSpinBox(xAxisGroupBox);
        QSpacerItem *horizontalLabelSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QCheckBox *checkShowAxisHorizontalGrid = new QCheckBox("Show horizontal grid", xAxisGroupBox);

        QHBoxLayout *horizontalGridColorLayout = new QHBoxLayout();
        QLabel *horizontalGridColorLabel = new QLabel("Grid color", xAxisGroupBox);
        QComboBox *horizontalGridColorCombo = new QComboBox(xAxisGroupBox);
        QSpacerItem *horizontalGridColorSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        xAxisGroupBox->setLayout(xAxisLayout);

        xAxisLayout->addLayout(rangeMinXLayout);
        rangeMinXLayout->addWidget(minXLabel);
        rangeMinXLayout->addWidget(minXEdit);
        rangeMinXLayout->addItem(horizontalMinXSpacer);

        xAxisLayout->addLayout(rangeMaxXLayout);
        rangeMaxXLayout->addWidget(maxXLabel);
        rangeMaxXLayout->addWidget(maxXEdit);
        rangeMaxXLayout->addItem(horizontalMaxXSpacer);

        xAxisLayout->addWidget(checkXAxisNameVisible);

        xAxisLayout->addLayout(xAxisNameLayout);
        xAxisNameLayout->addWidget(xAxisNameLabel);
        xAxisNameLayout->addWidget(xAxisNameEdit);
        xAxisNameLayout->addItem(xAxisNameSpacer);

        xAxisLayout->addLayout(xAxisNameSizeLayout);
        xAxisNameSizeLayout->addWidget(xAxisNameSizeLabel);
        xAxisNameSizeLayout->addWidget(xAxisNameSizeSpin);
        xAxisNameSizeLayout->addItem(xAxisNameSizeSpacer);

        xAxisLayout->addWidget(checkHorizontalLabelVisible);

        xAxisLayout->addLayout(horizontalLabelAngleLayout);
        horizontalLabelAngleLayout->addWidget(horizontalLabelAngleLabel);
        horizontalLabelAngleLayout->addWidget(horizontalLabelAngleEdit);
        horizontalLabelAngleLayout->addItem(horizontalLabelAngleSpacer);

        xAxisLayout->addLayout(horizontalLabelColorLayout);
        horizontalLabelColorLayout->addWidget(horizontalLabelColorLabel);
        horizontalLabelColorLayout->addWidget(horizontalLabelColorCombo);
        horizontalLabelColorLayout->addItem(horizontalLabelColorSpacer);

        xAxisLayout->addLayout(horizontalLabelSizeLayout);
        horizontalLabelSizeLayout->addWidget(horizontalLabelSizeLabel);
        horizontalLabelSizeLayout->addWidget(horizontalLabelSizeSpin);
        horizontalLabelSizeLayout->addItem(horizontalLabelSizeSpacer);

        xAxisLayout->addWidget(checkShowAxisHorizontalGrid);

        xAxisLayout->addLayout(horizontalGridColorLayout);
        horizontalGridColorLayout->addWidget(horizontalGridColorLabel);
        horizontalGridColorLayout->addWidget(horizontalGridColorCombo);
        horizontalGridColorLayout->addItem(horizontalGridColorSpacer);

        minXLabel->setMinimumWidth(LabelWidth);
        minXEdit->setMaximumWidth(EditWidth1);
        maxXLabel->setMinimumWidth(LabelWidth);
        maxXEdit->setMaximumWidth(EditWidth1);
        checkXAxisNameVisible->setChecked(true);
        xAxisNameLabel->setMinimumWidth(LabelWidth);
        xAxisNameSizeLabel->setMinimumWidth(LabelWidth);
        checkHorizontalLabelVisible->setChecked(true);
        horizontalLabelAngleEdit->setText("0");
        horizontalLabelAngleEdit->setMaximumWidth(EditWidth1);
        horizontalLabelAngleLabel->setMinimumWidth(LabelWidth);
        horizontalLabelColorLabel->setMinimumWidth(LabelWidth);
        horizontalLabelColorCombo->insertItems(0, colorNameList);
        horizontalLabelColorCombo->setCurrentIndex(2);               //black
        horizontalLabelSizeLabel->setMinimumWidth(LabelWidth);
        checkShowAxisHorizontalGrid->setChecked(true);
        horizontalGridColorLabel->setMinimumWidth(LabelWidth);
        horizontalGridColorCombo->insertItems(0, colorNameList);
        horizontalGridColorCombo->setCurrentIndex(6);                //lightGray

        if(plotTableRanges.size() > 0){
            auto setMinXEdit = [minXEdit, this](){ minXEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast())->min())); };
            auto setMaxXEdit = [maxXEdit, this](){ maxXEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast())->max())); };
            setMinXEdit();
            setMaxXEdit();
            xAxisNameSizeSpin->setValue(graph->axes(Qt::Horizontal).constLast()->titleFont().pointSize());
            horizontalLabelSizeSpin->setValue(graph->axes(Qt::Horizontal).constLast()->labelsFont().pointSize());

            connect(minXEdit, &QLineEdit::textEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setMin);
            connect(maxXEdit, &QLineEdit::textEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setMax);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast()), &QValueAxis::minChanged, setMinXEdit);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast()), &QValueAxis::maxChanged, setMaxXEdit);
            connect(this, &Graph2DSeries::updateGraphSeries, setMinXEdit);
            connect(this, &Graph2DSeries::updateGraphSeries, setMaxXEdit);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setTitleVisible);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, xAxisNameLabel, &QLabel::setVisible);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, xAxisNameEdit, &QLineEdit::setVisible);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, xAxisNameSizeLabel, &QLabel::setVisible);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, xAxisNameSizeSpin, &QSpinBox::setVisible);
            connect(xAxisNameEdit, &QLineEdit::textEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setTitleText);
            connect(xAxisNameSizeSpin, &QSpinBox::valueChanged, [this, xAxisNameSizeSpin](){
                QFont xAxisNameFont; xAxisNameFont.setPointSize(xAxisNameSizeSpin->value());
                graph->axes(Qt::Horizontal).constLast()->setTitleFont(xAxisNameFont);
            });
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setLabelsVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelAngleLabel, &QLabel::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelAngleEdit, &QLineEdit::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelColorLabel, &QLabel::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelColorCombo, &QComboBox::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelSizeLabel, &QLabel::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelSizeSpin, &QSpinBox::setVisible);
            connect(horizontalLabelAngleEdit, &QLineEdit::textEdited, [this, horizontalLabelAngleEdit](){
                graph->axes(Qt::Horizontal).constLast()->setLabelsAngle(horizontalLabelAngleEdit->text().toInt());
            });
            connect(horizontalLabelColorCombo, &QComboBox::currentIndexChanged, [this, horizontalLabelColorCombo](){
                graph->axes(Qt::Horizontal).constLast()->setLabelsColor(Qt::GlobalColor(horizontalLabelColorCombo->currentIndex()));
            });
            connect(horizontalLabelSizeSpin, &QSpinBox::valueChanged, [this, horizontalLabelSizeSpin](){
                QFont labelsFont; labelsFont.setPointSize(horizontalLabelSizeSpin->value());
                graph->axes(Qt::Horizontal).constLast()->setLabelsFont(labelsFont);
            });
            connect(checkShowAxisHorizontalGrid, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setGridLineVisible);
            connect(checkShowAxisHorizontalGrid, &QCheckBox::toggled, horizontalGridColorLabel, &QLabel::setVisible);
            connect(checkShowAxisHorizontalGrid, &QCheckBox::toggled, horizontalGridColorCombo, &QComboBox::setVisible);
            connect(horizontalGridColorCombo, &QComboBox::currentIndexChanged, [this, horizontalGridColorCombo](){
                graph->axes(Qt::Horizontal).constLast()->setGridLineColor(Qt::GlobalColor(horizontalGridColorCombo->currentIndex()));
            });
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

        QCheckBox *checkYAxisNameVisible = new QCheckBox("Show axis name", yAxisGroupBox);

        QHBoxLayout *yAxisNameLayout = new QHBoxLayout();
        QLabel *yAxisNameLabel = new QLabel("Axis name", yAxisGroupBox);
        QLineEdit *yAxisNameEdit = new QLineEdit(yAxisGroupBox);
        QSpacerItem *yAxisNameSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *yAxisNameSizeLayout = new QHBoxLayout();
        QLabel *yAxisNameSizeLabel = new QLabel("Axis name size", yAxisGroupBox);
        QSpinBox *yAxisNameSizeSpin = new QSpinBox(yAxisGroupBox);
        QSpacerItem *yAxisNameSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QCheckBox *checkVerticalLabelVisible = new QCheckBox("Show label", yAxisGroupBox);

        QHBoxLayout *verticalLabelAngleLayout = new QHBoxLayout();
        QLabel *verticalLabelAngleLabel = new QLabel("Label angle", yAxisGroupBox);
        QLineEdit *verticalLabelAngleEdit = new QLineEdit(yAxisGroupBox);
        QSpacerItem *verticalLabelAngleSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *verticalLabelColorLayout = new QHBoxLayout();
        QLabel *verticalLabelColorLabel = new QLabel("Label color", yAxisGroupBox);
        QComboBox *verticalLabelColorCombo = new QComboBox(yAxisGroupBox);
        QSpacerItem *verticalLabelColorSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *verticalLabelSizeLayout = new QHBoxLayout();
        QLabel *verticalLabelSizeLabel = new QLabel("Label size", yAxisGroupBox);
        QSpinBox *verticalLabelSizeSpin = new QSpinBox(yAxisGroupBox);
        QSpacerItem *verticalLabelSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QCheckBox *checkShowAxisVerticalGrid = new QCheckBox("Show vertical grid", xAxisGroupBox);

        QHBoxLayout *verticalGridColorLayout = new QHBoxLayout();
        QLabel *verticalGridColorLabel = new QLabel("Grid color", yAxisGroupBox);
        QComboBox *verticalGridColorCombo = new QComboBox(yAxisGroupBox);
        QSpacerItem *verticalGridColorSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        yAxisGroupBox->setLayout(yAxisLayout);

        yAxisLayout->addLayout(rangeMinYLayout);
        rangeMinYLayout->addWidget(minYLabel);
        rangeMinYLayout->addWidget(minYEdit);
        rangeMinYLayout->addItem(horizontalMinYSpacer);

        yAxisLayout->addLayout(rangeMaxYLayout);
        rangeMaxYLayout->addWidget(maxYLabel);
        rangeMaxYLayout->addWidget(maxYEdit);
        rangeMaxYLayout->addItem(horizontalMaxYSpacer);

        yAxisLayout->addWidget(checkYAxisNameVisible);

        yAxisLayout->addLayout(yAxisNameLayout);
        yAxisNameLayout->addWidget(yAxisNameLabel);
        yAxisNameLayout->addWidget(yAxisNameEdit);
        yAxisNameLayout->addItem(yAxisNameSpacer);

        yAxisLayout->addLayout(yAxisNameSizeLayout);
        yAxisNameSizeLayout->addWidget(yAxisNameSizeLabel);
        yAxisNameSizeLayout->addWidget(yAxisNameSizeSpin);
        yAxisNameSizeLayout->addItem(yAxisNameSizeSpacer);

        yAxisLayout->addWidget(checkVerticalLabelVisible);

        yAxisLayout->addLayout(verticalLabelAngleLayout);
        verticalLabelAngleLayout->addWidget(verticalLabelAngleLabel);
        verticalLabelAngleLayout->addWidget(verticalLabelAngleEdit);
        verticalLabelAngleLayout->addItem(verticalLabelAngleSpacer);

        yAxisLayout->addLayout(verticalLabelColorLayout);
        verticalLabelColorLayout->addWidget(verticalLabelColorLabel);
        verticalLabelColorLayout->addWidget(verticalLabelColorCombo);
        verticalLabelColorLayout->addItem(verticalLabelColorSpacer);

        yAxisLayout->addLayout(verticalLabelSizeLayout);
        verticalLabelSizeLayout->addWidget(verticalLabelSizeLabel);
        verticalLabelSizeLayout->addWidget(verticalLabelSizeSpin);
        verticalLabelSizeLayout->addItem(verticalLabelSizeSpacer);

        yAxisLayout->addWidget(checkShowAxisVerticalGrid);

        yAxisLayout->addLayout(verticalGridColorLayout);
        verticalGridColorLayout->addWidget(verticalGridColorLabel);
        verticalGridColorLayout->addWidget(verticalGridColorCombo);
        verticalGridColorLayout->addItem(verticalGridColorSpacer);

        minYLabel->setMinimumWidth(LabelWidth);
        minYEdit->setMaximumWidth(EditWidth1);
        maxYLabel->setMinimumWidth(LabelWidth);
        maxYEdit->setMaximumWidth(EditWidth1);
        checkYAxisNameVisible->setChecked(true);
        yAxisNameLabel->setMinimumWidth(LabelWidth);
        yAxisNameSizeLabel->setMinimumWidth(LabelWidth);
        checkVerticalLabelVisible->setChecked(true);
        verticalLabelAngleLabel->setMinimumWidth(LabelWidth);
        verticalLabelAngleEdit->setText("0");
        verticalLabelAngleEdit->setMaximumWidth(EditWidth1);
        verticalLabelColorLabel->setMinimumWidth(LabelWidth);
        verticalLabelColorCombo->insertItems(0, colorNameList);
        verticalLabelColorCombo->setCurrentIndex(2);              //black
        verticalLabelSizeLabel->setMinimumWidth(LabelWidth);
        checkShowAxisVerticalGrid->setChecked(true);
        verticalGridColorLabel->setMinimumWidth(LabelWidth);
        verticalGridColorCombo->insertItems(0, colorNameList);
        verticalGridColorCombo->setCurrentIndex(6);               //lightGray

        if(plotTableRanges.size() > 0){
            auto setMinYEdit = [this, minYEdit](){ minYEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast())->min())); };
            auto setMaxYEdit = [this, maxYEdit](){ maxYEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast())->max())); };
            setMinYEdit();
            setMaxYEdit();
            yAxisNameSizeSpin->setValue(graph->axes(Qt::Vertical).constLast()->titleFont().pointSize());
            verticalLabelSizeSpin->setValue(graph->axes(Qt::Vertical).constLast()->labelsFont().pointSize());

            connect(minYEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setMin);
            connect(maxYEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setMax);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast()), &QValueAxis::minChanged, setMinYEdit);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast()), &QValueAxis::maxChanged, setMaxYEdit);
            connect(this, &Graph2DSeries::updateGraphSeries, setMinYEdit);
            connect(this, &Graph2DSeries::updateGraphSeries, setMaxYEdit);
            connect(checkYAxisNameVisible, &QCheckBox::toggled, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setTitleVisible);
            connect(checkYAxisNameVisible, &QCheckBox::toggled, yAxisNameLabel, &QLabel::setVisible);
            connect(checkYAxisNameVisible, &QCheckBox::toggled, yAxisNameEdit, &QLineEdit::setVisible);
            connect(checkYAxisNameVisible, &QCheckBox::toggled, yAxisNameSizeLabel, &QLabel::setVisible);
            connect(checkYAxisNameVisible, &QCheckBox::toggled, yAxisNameSizeSpin, &QSpinBox::setVisible);
            connect(yAxisNameEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setTitleText);
            connect(yAxisNameSizeSpin, &QSpinBox::valueChanged, [this, yAxisNameSizeSpin](){
                QFont yAxisNameFont; yAxisNameFont.setPointSize(yAxisNameSizeSpin->value());
                graph->axes(Qt::Vertical).constLast()->setTitleFont(yAxisNameFont);
            });
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setVisible);
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, verticalLabelAngleLabel, &QLabel::setVisible);
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, verticalLabelAngleEdit, &QLineEdit::setVisible);
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, verticalLabelColorLabel, &QLabel::setVisible);
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, verticalLabelColorCombo, &QComboBox::setVisible);
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, verticalLabelSizeLabel, &QLabel::setVisible);
            connect(checkVerticalLabelVisible, &QCheckBox::toggled, verticalLabelSizeSpin, &QSpinBox::setVisible);
            connect(verticalLabelAngleEdit, &QLineEdit::textEdited, [this, verticalLabelAngleEdit](){
                graph->axes(Qt::Vertical).constLast()->setLabelsAngle(verticalLabelAngleEdit->text().toInt());
            });
            connect(verticalLabelColorCombo, &QComboBox::currentIndexChanged, [this, verticalLabelColorCombo](){
                graph->axes(Qt::Vertical).constLast()->setLabelsColor(Qt::GlobalColor(verticalLabelColorCombo->currentIndex()));
            });
            connect(verticalLabelSizeSpin, &QSpinBox::valueChanged, [this, verticalLabelSizeSpin](){
                QFont labelsFont; labelsFont.setPointSize(verticalLabelSizeSpin->value());
                graph->axes(Qt::Vertical).constLast()->setLabelsFont(labelsFont);
            });
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setGridLineVisible);
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, verticalGridColorLabel, &QLabel::setVisible);
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, verticalGridColorCombo, &QComboBox::setVisible);
            connect(verticalGridColorCombo, &QComboBox::currentIndexChanged, [this, verticalGridColorCombo](){
                graph->axes(Qt::Vertical).constLast()->setGridLineColor(Qt::GlobalColor(verticalGridColorCombo->currentIndex()));
            });
        }
        //graph->axes(Qt::Vertical).constLast()->setShadesVisible(true);
    }
    {   /* グラフの出力設定項目 */
        QWidget *exportSettingWidget = new QWidget(settingStackWidget);
        QVBoxLayout *exportSettingLayout = new QVBoxLayout(exportSettingWidget);

        QHBoxLayout *exportFileNameLayout = new QHBoxLayout();
        QLabel *exportFileNameLabel = new QLabel("File name", exportSettingWidget);
        QLineEdit *exportFileNameEdit = new QLineEdit("file", exportSettingWidget);
        QSpacerItem *exportFileNameSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QHBoxLayout *exportTypeLayout = new QHBoxLayout();
        QLabel *exportTypeLabel = new QLabel("Export to", exportSettingWidget);
        QComboBox *exportTypeCombo = new QComboBox(exportSettingWidget);
        QSpacerItem *exportTypeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QStackedWidget *exportStackWidget = new QStackedWidget(exportSettingWidget);

        QSpacerItem *exportSettingSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        settingStackWidget->addWidget(exportSettingWidget);
        exportSettingWidget->setLayout(exportSettingLayout);

        exportSettingLayout->addLayout(exportFileNameLayout);
        exportFileNameLayout->addWidget(exportFileNameLabel);
        exportFileNameLayout->addWidget(exportFileNameEdit);
        exportFileNameLayout->addItem(exportFileNameSpacer);

        exportSettingLayout->addLayout(exportTypeLayout);
        exportTypeLayout->addWidget(exportTypeLabel);
        exportTypeLayout->addWidget(exportTypeCombo);
        exportTypeLayout->addItem(exportTypeSpacer);

        exportSettingLayout->addWidget(exportStackWidget);

        exportSettingLayout->addItem(exportSettingSpacer);

        exportFileNameLabel->setMinimumWidth(LabelWidth);
        exportTypeLabel->setMinimumWidth(LabelWidth);
        exportTypeCombo->insertItems(0, QStringList() << "Image");

        connect(exportTypeCombo, &QComboBox::currentIndexChanged, exportStackWidget, &QStackedWidget::setCurrentIndex);

        { /* Image */
            QGroupBox *exportImageWidget = new QGroupBox("Image", exportStackWidget);
            QVBoxLayout *exportImageLayout = new QVBoxLayout(exportImageWidget);

            QHBoxLayout *imageFormatLayout = new QHBoxLayout();
            QLabel *imageFormatLabel = new QLabel("Format", exportImageWidget);
            QComboBox *imageFormatCombo = new QComboBox(exportImageWidget);
            QSpacerItem *imageFormatSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

            QSpacerItem *imageExportFixedSpacer = new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

            QPushButton *imageExportButton = new QPushButton("Export", exportImageWidget);

            QSpacerItem *exportImageSpacer = new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Expanding);

            exportStackWidget->addWidget(exportImageWidget);
            exportImageWidget->setLayout(exportImageLayout);

            exportImageLayout->addLayout(imageFormatLayout);
            imageFormatLayout->addWidget(imageFormatLabel);
            imageFormatLayout->addWidget(imageFormatCombo);
            imageFormatLayout->addItem(imageFormatSpacer);

            exportImageLayout->addItem(imageExportFixedSpacer);

            exportImageLayout->addWidget(imageExportButton);

            exportImageLayout->addItem(exportImageSpacer);

            imageFormatLabel->setMinimumWidth(LabelWidth);
            imageFormatCombo->insertItems(0, imgFormatList());

            connect(imageExportButton, &QPushButton::released, [=](){
                const QString direcotryPath = QFileDialog::getExistingDirectory(this);
                const QString imageFormat = imageFormatCombo->currentText();

                QImage img = graphView->grab().toImage();
                const bool success = img.save(direcotryPath + "/" + exportFileNameEdit->text() + "." + imageFormat, imageFormat.toLocal8Bit().constData());
                if(!success){
                    QMessageBox::critical(this, "Error", "Failed to save.                                            ");
                }
            });
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

    emit updateGraphSeries();
}

void Graph2DSeries::changeLegendVisible(bool visible)
{
    graph->legend()->setVisible(visible);
    for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        legendNameEdit[i]->setVisible(visible);
}

const QList<QString> Graph2DSeries::colorNameList =
{
    "color0",
    "color1",
    "black",
    "white",
    "darkGray",
    "gray",
    "lightGray",
    "red",
    "green",
    "blue",
    "cyan",
    "magenta",
    "yellow",
    "darkRed",
    "darkGreen",
    "darkBlue",
    "darkCyan",
    "darkMagenta",
    "darkYello",
    "transparent"
};

const QList<QString> Graph2DSeries::themeNameList =
{
    "light",
    "blueCerulean",
    "dark",
    "brownSand",
    "blueNcs",
    "highContrast",
    "blueIcy",
    "qt"
};

const QList<QString> Graph2DSeries::imgFormatList()
{
    QList<QString> imgFormatList;
    for(const QByteArray& byte : QImageWriter::supportedImageFormats())
    {
        imgFormatList << byte.constData();
    }

    return imgFormatList;
}
