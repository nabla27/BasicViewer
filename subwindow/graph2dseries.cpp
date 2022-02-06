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

#define QT_GLOBAL_COLOR_COUNT 19
#define SETTING_LABEL_WIDTH 80
#define SETTING_EDIT_WIDTH 35


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

    raise();

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
    {
        selectPageCombo->addItem("Graph");
        selectPageCombo->addItem("Series");
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
        //追加するレイアウト
        LineEditLayout *titleEditLayout = new LineEditLayout(graphSettingWidget, "Title");
        ComboEditLayout *themeSetLayout = new ComboEditLayout(graphSettingWidget, "Theme");
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        settingStackWidget->addWidget(graphSettingWidget);
        graphSettingWidget->setLayout(graphSettingLayout);

        graphSettingLayout->addLayout(titleEditLayout);
        graphSettingLayout->addLayout(themeSetLayout);
        graphSettingLayout->addItem(verticalSpacer);

        themeSetLayout->insertComboItems(0, themeNameList);

        connect(titleEditLayout, &LineEditLayout::lineTextEdited, graph, &QChart::setTitle);
        connect(themeSetLayout, &ComboEditLayout::currentComboIndexChanged, [this, themeSetLayout](){
            graph->setTheme(QChart::ChartTheme(themeSetLayout->currentComboIndex()));
        });
    }
    {
        /* seriesの設定項目 */
        QWidget *seriesSettingWidget = new QWidget(settingStackWidget);
        QVBoxLayout *seriesSettingLayout = new QVBoxLayout(seriesSettingWidget);
        QTabWidget *seriesSettingTab = new QTabWidget(seriesSettingWidget);
        QSpacerItem *seriesSettingSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        settingStackWidget->addWidget(seriesSettingWidget);
        seriesSettingWidget->setLayout(seriesSettingLayout);
        seriesSettingLayout->addWidget(seriesSettingTab);
        seriesSettingLayout->addItem(seriesSettingSpacer);

        for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        {
            QWidget *tabWidget = new QWidget(seriesSettingTab);
            QVBoxLayout *tabWidgetLayout = new QVBoxLayout(tabWidget);
            QSpacerItem *tabSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

            ComboEditLayout *seriesLineColorLayout = new ComboEditLayout(tabWidget, "Line color");
            RGBEditLayout *seriesLineColorCustomLayout = new RGBEditLayout(tabWidget);

            seriesSettingTab->addTab(tabWidget, "series " + QString::number(i));
            tabWidget->setLayout(tabWidgetLayout);

            tabWidgetLayout->addLayout(seriesLineColorLayout);
            tabWidgetLayout->addLayout(seriesLineColorCustomLayout);
            tabWidgetLayout->addItem(tabSpacer);

            seriesLineColorLayout->insertComboItems(0, colorNameList);
            seriesLineColorCustomLayout->setColor(qobject_cast<QXYSeries*>(graph->series().at(i))->color());

            connect(seriesLineColorLayout, &ComboEditLayout::currentComboIndexChanged, [=](){
                const int index = seriesLineColorLayout->currentComboIndex();
                if(index > QT_GLOBAL_COLOR_COUNT) { seriesLineColorCustomLayout->setReadOnly(false); return; }
                qobject_cast<QXYSeries*>(graph->series().at(i))->setColor(Qt::GlobalColor(index));
                seriesLineColorCustomLayout->setColor(index);
                seriesLineColorCustomLayout->setReadOnly(true);
            });
            connect(seriesLineColorCustomLayout, &RGBEditLayout::colorEdited,
                    qobject_cast<QXYSeries*>(graph->series().at(i)), &QXYSeries::setColor);
        }
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
        QTabWidget *labelTab = new QTabWidget(labelSettingWidget);

        settingStackWidget->addWidget(labelSettingWidget);
        labelSettingWidget->setLayout(labelSettingLayout);
        labelSettingLayout->addWidget(labelTab);

        for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        {
            QWidget *toolBoxWidget = new QWidget(labelTab);
            QVBoxLayout *toolBoxLayout = new QVBoxLayout(toolBoxWidget);
            QCheckBox *checkLabelVisible = new QCheckBox("Show label", toolBoxWidget);
            QCheckBox *checkLabelPointsVisible = new QCheckBox("Show label points", toolBoxWidget);
            QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

            labelTab->addTab(toolBoxWidget, "series " + QString::number(i));
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
        LineEditLayout *rangeMinXLayout = new LineEditLayout(xAxisGroupBox, "Min");
        LineEditLayout *rangeMaxXLayout = new LineEditLayout(xAxisGroupBox, "Max");
        QCheckBox *checkXAxisNameVisible = new QCheckBox("Show axis name", xAxisGroupBox);
        LineEditLayout *xAxisNameLayout = new LineEditLayout(xAxisGroupBox, "Axis name");
        SpinBoxEditLayout *xAxisNameSizeLayout = new SpinBoxEditLayout(xAxisGroupBox, "Axis name size");
        QCheckBox *checkHorizontalLabelVisible = new QCheckBox("Show label", yAxisGroupBox);
        LineEditLayout *horizontalLabelAngleLayout = new LineEditLayout(xAxisGroupBox, "Label Angle");
        ComboEditLayout *horizontalLabelColorLayout = new ComboEditLayout(xAxisGroupBox, "Label color");
        RGBEditLayout *horizontalLabelColorCustom = new RGBEditLayout(xAxisGroupBox);
        SpinBoxEditLayout *horizontalLabelSizeLayout = new SpinBoxEditLayout(xAxisGroupBox, "Label size");
        QCheckBox *checkShowAxisHorizontalGrid = new QCheckBox("Show horizontal grid", xAxisGroupBox);
        ComboEditLayout *horizontalGridColorLayout = new ComboEditLayout(xAxisGroupBox, "Grid color");
        RGBEditLayout *horizontalGridColorCustom = new RGBEditLayout(xAxisGroupBox);

        xAxisGroupBox->setLayout(xAxisLayout);
        xAxisLayout->addLayout(rangeMinXLayout);
        xAxisLayout->addLayout(rangeMaxXLayout);
        xAxisLayout->addWidget(checkXAxisNameVisible);
        xAxisLayout->addLayout(xAxisNameLayout);
        xAxisLayout->addLayout(xAxisNameSizeLayout);
        xAxisLayout->addWidget(checkHorizontalLabelVisible);
        xAxisLayout->addLayout(horizontalLabelAngleLayout);
        xAxisLayout->addLayout(horizontalLabelColorLayout);
        xAxisLayout->addLayout(horizontalLabelColorCustom);
        xAxisLayout->addLayout(horizontalLabelSizeLayout);
        xAxisLayout->addWidget(checkShowAxisHorizontalGrid);
        xAxisLayout->addLayout(horizontalGridColorLayout);
        xAxisLayout->addLayout(horizontalGridColorCustom);

        rangeMinXLayout->setLineEditMaximumWidth(SETTING_EDIT_WIDTH);
        rangeMaxXLayout->setLineEditMaximumWidth(SETTING_EDIT_WIDTH);
        checkXAxisNameVisible->setChecked(true);
        checkHorizontalLabelVisible->setChecked(true);
        horizontalLabelColorLayout->insertComboItems(0, colorNameList);
        horizontalLabelColorLayout->setComboCurrentIndex(Qt::black);
        checkShowAxisHorizontalGrid->setChecked(true);
        horizontalGridColorLayout->insertComboItems(0, colorNameList);
        horizontalGridColorLayout->setComboCurrentIndex(Qt::lightGray);

        if(plotTableRanges.size() > 0)
        {
            auto setMinXEdit = [rangeMinXLayout, this](){ rangeMinXLayout->setLineEditText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast())->min())); };
            auto setMaxXEdit = [rangeMaxXLayout, this](){ rangeMaxXLayout->setLineEditText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast())->max())); };
            setMinXEdit();
            setMaxXEdit();
            xAxisNameSizeLayout->setSpinBoxValue(graph->axes(Qt::Horizontal).constLast()->titleFont().pointSize());
            horizontalLabelAngleLayout->setLineEditText(QString::number(graph->axes(Qt::Horizontal).constLast()->labelsAngle()));
            horizontalLabelSizeLayout->setSpinBoxValue(graph->axes(Qt::Horizontal).constLast()->labelsFont().pointSize());
            horizontalLabelColorCustom->setColor(graph->axes(Qt::Horizontal).constLast()->labelsColor());
            horizontalGridColorCustom->setColor(graph->axes(Qt::Horizontal).constLast()->gridLineColor());

            connect(rangeMinXLayout, &LineEditLayout::lineTextEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setMin);
            connect(rangeMaxXLayout, &LineEditLayout::lineTextEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setMax);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast()), &QValueAxis::minChanged, setMinXEdit);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Horizontal).constLast()), &QValueAxis::maxChanged, setMaxXEdit);
            connect(this, &Graph2DSeries::graphSeriesUpdated, setMinXEdit);
            connect(this, &Graph2DSeries::graphSeriesUpdated, setMaxXEdit);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setTitleVisible);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, xAxisNameLayout, &LineEditLayout::setVisible);
            connect(checkXAxisNameVisible, &QCheckBox::toggled, xAxisNameSizeLayout, &SpinBoxEditLayout::setVisible);
            connect(xAxisNameLayout, &LineEditLayout::lineTextEdited, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setTitleText);
            connect(xAxisNameSizeLayout, &SpinBoxEditLayout::spinBoxValueChanged, [this, xAxisNameSizeLayout](){
                QFont xAxisNameFont = graph->axes(Qt::Horizontal).constLast()->titleFont();
                xAxisNameFont.setPointSize(xAxisNameSizeLayout->spinBoxValue());
                graph->axes(Qt::Horizontal).constLast()->setTitleFont(xAxisNameFont);
            });
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setLabelsVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelAngleLayout, &LineEditLayout::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelColorLayout, &ComboEditLayout::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelColorCustom, &RGBEditLayout::setVisible);
            connect(checkHorizontalLabelVisible, &QCheckBox::toggled, horizontalLabelSizeLayout, &SpinBoxEditLayout::setVisible);
            connect(horizontalLabelAngleLayout, &LineEditLayout::lineTextEdited, [this, horizontalLabelAngleLayout](){
                graph->axes(Qt::Horizontal).constLast()->setLabelsAngle(horizontalLabelAngleLayout->lineEditText().toInt());
            });
            connect(horizontalLabelColorLayout, &ComboEditLayout::currentComboIndexChanged, [this, horizontalLabelColorLayout, horizontalLabelColorCustom](){
                const int index = horizontalLabelColorLayout->currentComboIndex();
                if(index > QT_GLOBAL_COLOR_COUNT) { horizontalLabelColorCustom->setReadOnly(false); return; }
                graph->axes(Qt::Horizontal).constLast()->setLabelsColor(Qt::GlobalColor(index));
                horizontalLabelColorCustom->setColor(index);
                horizontalLabelColorCustom->setReadOnly(true);
            });
            connect(horizontalLabelColorCustom, &RGBEditLayout::colorEdited, graph->axes(Qt::Horizontal).constLast(), &QValueAxis::setLabelsColor);
            connect(horizontalLabelSizeLayout, &SpinBoxEditLayout::spinBoxValueChanged, [this, horizontalLabelSizeLayout](){
                QFont labelsFont = graph->axes(Qt::Horizontal).constLast()->labelsFont();
                labelsFont.setPointSize(horizontalLabelSizeLayout->spinBoxValue());
                graph->axes(Qt::Horizontal).constLast()->setLabelsFont(labelsFont);
            });
            connect(checkShowAxisHorizontalGrid, &QCheckBox::toggled, graph->axes(Qt::Horizontal).constLast(), &QAbstractAxis::setGridLineVisible);
            connect(checkShowAxisHorizontalGrid, &QCheckBox::toggled, horizontalGridColorLayout, &ComboEditLayout::setVisible);
            connect(horizontalGridColorLayout, &ComboEditLayout::currentComboIndexChanged, [this, horizontalGridColorLayout, horizontalGridColorCustom](){
                const int index = horizontalGridColorLayout->currentComboIndex();
                if(index > QT_GLOBAL_COLOR_COUNT) { horizontalGridColorCustom->setReadOnly(false); return; }
                graph->axes(Qt::Horizontal).constLast()->setGridLineColor(Qt::GlobalColor(index));
                horizontalGridColorCustom->setColor(index);
                horizontalGridColorCustom->setReadOnly(true);
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

        RGBEditLayout *verticalLabelColorCustom = new RGBEditLayout(yAxisGroupBox);

        QHBoxLayout *verticalLabelSizeLayout = new QHBoxLayout();
        QLabel *verticalLabelSizeLabel = new QLabel("Label size", yAxisGroupBox);
        QSpinBox *verticalLabelSizeSpin = new QSpinBox(yAxisGroupBox);
        QSpacerItem *verticalLabelSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        QCheckBox *checkShowAxisVerticalGrid = new QCheckBox("Show vertical grid", xAxisGroupBox);

        QHBoxLayout *verticalGridColorLayout = new QHBoxLayout();
        QLabel *verticalGridColorLabel = new QLabel("Grid color", yAxisGroupBox);
        QComboBox *verticalGridColorCombo = new QComboBox(yAxisGroupBox);
        QSpacerItem *verticalGridColorSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        RGBEditLayout *verticalGridColorCustom = new RGBEditLayout(yAxisGroupBox);

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

        yAxisLayout->addLayout(verticalLabelColorCustom);

        yAxisLayout->addLayout(verticalLabelSizeLayout);
        verticalLabelSizeLayout->addWidget(verticalLabelSizeLabel);
        verticalLabelSizeLayout->addWidget(verticalLabelSizeSpin);
        verticalLabelSizeLayout->addItem(verticalLabelSizeSpacer);

        yAxisLayout->addWidget(checkShowAxisVerticalGrid);

        yAxisLayout->addLayout(verticalGridColorLayout);
        verticalGridColorLayout->addWidget(verticalGridColorLabel);
        verticalGridColorLayout->addWidget(verticalGridColorCombo);
        verticalGridColorLayout->addItem(verticalGridColorSpacer);

        yAxisLayout->addLayout(verticalGridColorCustom);

        minYLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        minYEdit->setMaximumWidth(SETTING_EDIT_WIDTH);
        maxYLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        maxYEdit->setMaximumWidth(SETTING_EDIT_WIDTH);
        checkYAxisNameVisible->setChecked(true);
        yAxisNameLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        yAxisNameSizeLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        checkVerticalLabelVisible->setChecked(true);
        verticalLabelAngleLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        verticalLabelAngleEdit->setText("0");
        verticalLabelAngleEdit->setMaximumWidth(SETTING_EDIT_WIDTH);
        verticalLabelColorLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        verticalLabelColorCombo->insertItems(0, colorNameList);
        verticalLabelColorCombo->setCurrentIndex(Qt::black);
        verticalLabelColorCustom->setLabelMinimumWidth(SETTING_LABEL_WIDTH);
        verticalLabelColorCustom->setEditMaximumWidth(SETTING_EDIT_WIDTH - 10);
        verticalLabelColorCustom->setReadOnly(true);
        verticalLabelSizeLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        checkShowAxisVerticalGrid->setChecked(true);
        verticalGridColorLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        verticalGridColorCombo->insertItems(0, colorNameList);
        verticalGridColorCombo->setCurrentIndex(Qt::lightGray);
        verticalGridColorCustom->setLabelMinimumWidth(SETTING_LABEL_WIDTH);
        verticalGridColorCustom->setEditMaximumWidth(SETTING_EDIT_WIDTH - 10);
        verticalGridColorCustom->setReadOnly(true);

        if(plotTableRanges.size() > 0){
            auto setMinYEdit = [this, minYEdit](){ minYEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast())->min())); };
            auto setMaxYEdit = [this, maxYEdit](){ maxYEdit->setText(QString::number(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast())->max())); };
            setMinYEdit();
            setMaxYEdit();
            yAxisNameSizeSpin->setValue(graph->axes(Qt::Vertical).constLast()->titleFont().pointSize());
            verticalLabelSizeSpin->setValue(graph->axes(Qt::Vertical).constLast()->labelsFont().pointSize());
            verticalLabelColorCustom->setColor(graph->axes(Qt::Vertical).constLast()->labelsColor());
            verticalGridColorCustom->setColor(graph->axes(Qt::Vertical).constLast()->gridLineColor());

            connect(minYEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setMin);
            connect(maxYEdit, &QLineEdit::textEdited, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setMax);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast()), &QValueAxis::minChanged, setMinYEdit);
            connect(qobject_cast<QValueAxis*>(graph->axes(Qt::Vertical).constLast()), &QValueAxis::maxChanged, setMaxYEdit);
            connect(this, &Graph2DSeries::graphSeriesUpdated, setMinYEdit);
            connect(this, &Graph2DSeries::graphSeriesUpdated, setMaxYEdit);
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
            connect(verticalLabelColorCombo, &QComboBox::currentIndexChanged, [this, verticalLabelColorCombo, verticalLabelColorCustom](){
                const int index = verticalLabelColorCombo->currentIndex();
                if(index > QT_GLOBAL_COLOR_COUNT) { verticalLabelColorCustom->setReadOnly(false); return; }
                graph->axes(Qt::Vertical).constLast()->setLabelsColor(Qt::GlobalColor(index));
                verticalLabelColorCustom->setColor(index);
                verticalLabelColorCustom->setReadOnly(true);
            });
            connect(verticalLabelSizeSpin, &QSpinBox::valueChanged, [this, verticalLabelSizeSpin](){
                QFont labelsFont; labelsFont.setPointSize(verticalLabelSizeSpin->value());
                graph->axes(Qt::Vertical).constLast()->setLabelsFont(labelsFont);
            });
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, graph->axes(Qt::Vertical).constLast(), &QAbstractAxis::setGridLineVisible);
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, verticalGridColorLabel, &QLabel::setVisible);
            connect(checkShowAxisVerticalGrid, &QCheckBox::toggled, verticalGridColorCombo, &QComboBox::setVisible);
            connect(verticalGridColorCombo, &QComboBox::currentIndexChanged, [this, verticalGridColorCombo, verticalGridColorCustom](){
                const int index = verticalGridColorCombo->currentIndex();
                if(index > QT_GLOBAL_COLOR_COUNT) { verticalGridColorCustom->setReadOnly(false); return; }
                graph->axes(Qt::Vertical).constLast()->setGridLineColor(Qt::GlobalColor(index));
                verticalGridColorCustom->setColor(index);
                verticalGridColorCustom->setReadOnly(true);
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

        exportFileNameLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
        exportTypeLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
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

            imageFormatLabel->setMinimumWidth(SETTING_LABEL_WIDTH);
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
        //QLineSeries *series = new QLineSeries;
        QSplineSeries *series = new QSplineSeries;

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

    emit graphSeriesUpdated();          //グラフのデータ更新(それに伴う軸の更新も)のシグナルを送る。createDefaultAxesの後にシグナルを送る
}

void Graph2DSeries::changeLegendVisible(bool visible)
{
    graph->legend()->setVisible(visible);
    for(qsizetype i = 0; i < plotTableRanges.size(); ++i)
        legendNameEdit[i]->setVisible(visible);
}

const QList<QString> Graph2DSeries::colorNameList =
{
    "color0",        //0
    "color1",
    "black",
    "white",
    "darkGray",
    "gray",          //5
    "lightGray",
    "red",
    "green",
    "blue",
    "cyan",          //10
    "magenta",
    "yellow",
    "darkRed",
    "darkGreen",
    "darkBlue",      //15
    "darkCyan",
    "darkMagenta",
    "darkYello",
    "transparent",
    "custom",        //20
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

const QColor Graph2DSeries::rgbStrToColor(QString str)
{
    const QStringList rgbList = str.remove(' ').split(";");
    const qsizetype count = rgbList.size();

    const int red = (count > 0) ? rgbList.at(0).toInt() : 0;
    const int green = (count > 1) ? rgbList.at(1).toInt() : 0;
    const int blue = (count > 2) ? rgbList.at(1).toInt() : 0;

    return QColor(red, green, blue);
}






















RGBEditLayout::RGBEditLayout(QWidget *parent)
{
    label = new QLabel(parent);
    rEdit = new QLineEdit(parent);
    gEdit = new QLineEdit(parent);
    bEdit = new QLineEdit(parent);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    addWidget(label);
    addWidget(rEdit);
    addWidget(gEdit);
    addWidget(bEdit);
    addItem(spacer);

    setReadOnly(true);
    setEditMaximumWidth(SETTING_EDIT_WIDTH - 10);
    setLabelMinimumWidth(SETTING_LABEL_WIDTH);

    connect(rEdit, &QLineEdit::textEdited, [this](){ emit colorEdited(getColor()); });
    connect(gEdit, &QLineEdit::textEdited, [this](){ emit colorEdited(getColor()); });
    connect(bEdit, &QLineEdit::textEdited, [this](){ emit colorEdited(getColor()); });
}

void RGBEditLayout::setColor(const QColor& color)
{
    rEdit->setText(QString::number(color.red()));
    gEdit->setText(QString::number(color.green()));
    bEdit->setText(QString::number(color.blue()));
}

void RGBEditLayout::setColor(int eNum)
{
    QColor color = Qt::GlobalColor(eNum);
    rEdit->setText(QString::number(color.red()));
    gEdit->setText(QString::number(color.green()));
    bEdit->setText(QString::number(color.blue()));
}

void RGBEditLayout::setVisible(bool visible)
{
    label->setVisible(visible);
    rEdit->setVisible(visible);
    gEdit->setVisible(visible);
    bEdit->setVisible(visible);
}

void RGBEditLayout::setReadOnly(bool readOnly)
{
    rEdit->setReadOnly(readOnly);
    gEdit->setReadOnly(readOnly);
    bEdit->setReadOnly(readOnly);
}





ComboEditLayout::ComboEditLayout(QWidget *parent, const QString& text)
{
    label = new QLabel(text, parent);
    combo = new QComboBox(parent);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    addWidget(label);
    addWidget(combo);
    addItem(spacer);

    setLabelMinimumWidth(SETTING_LABEL_WIDTH);

    connect(combo, &QComboBox::currentIndexChanged, [this](){ emit currentComboIndexChanged(combo->currentIndex()); });
}

void ComboEditLayout::setVisible(bool visible)
{
    label->setVisible(visible);
    combo->setVisible(visible);
}





LineEditLayout::LineEditLayout(QWidget *parent, const QString& text)
{
    label = new QLabel(text, parent);
    lineEdit = new QLineEdit(parent);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    addWidget(label);
    addWidget(lineEdit);
    addItem(spacer);

    setLabelMinimumWidth(SETTING_LABEL_WIDTH);

    connect(lineEdit, &QLineEdit::textEdited, [this](){ emit lineTextEdited(lineEdit->text()); });
}

void LineEditLayout::setVisible(bool visible)
{
    label->setVisible(visible);
    lineEdit->setVisible(visible);
}






SpinBoxEditLayout::SpinBoxEditLayout(QWidget *parent, const QString& text)
{
    label = new QLabel(text, parent);
    spinBox = new QSpinBox(parent);
    spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    addWidget(label);
    addWidget(spinBox);
    addItem(spacer);

    setLabelMinimumWidth(SETTING_LABEL_WIDTH);

    connect(spinBox, &QSpinBox::valueChanged, [this](){ emit spinBoxValueChanged(spinBox->value()); });
}

void SpinBoxEditLayout::setVisible(bool visible)
{
    label->setVisible(visible);
    spinBox->setVisible(visible);
}







