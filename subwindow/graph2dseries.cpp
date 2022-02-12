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
 * QAbstractBarSeries(QBarCategoryAxis) ---|--- QBarSeries
 *                                         |--- QHorizontalBarSeries
 *                                         |--- QHorizontalPercentBarSeries
 *                                         |--- QHorizontalStackedBarSeries
 *                                         |--- QPercentBarSeries
 *                                         |--- QStackedBarSeries
 * QAreaSeries(QValueAxis)
 * QBoxPlotSeries(QBarCategoryAxis)
 * QCandlestickSeries(QBarCategoryAxis)
 * QPieSeries
 * QXYSeries(QValueAxis) ------------------|--- QLineSeries
 *                                         |--- QSplineSeries
 *                                         |--- QScatterSeries
 *
 * QBarCategoryAxis
 * QColorAxis
 * QDateTimeAxis
 * QLogValueAxis
 * QValueAxis
 *
 *
 */

#define SETTING_EDIT_LWIDTH 110
#define SETTING_EDIT_SWIDTH 35
#define QT_GLOBAL_COLOR_COUNT 19
#define SETTING_LABEL_WIDTH 80


Graph2DSeries::Graph2DSeries(TableWidget *table, QWidget *parent)
    : QWidget(parent), table(table)
{
    graph = new QChart;

    /* tableWidgetの選択範囲を設定 */
    setTableSelectedIndex();

    /* グラフデータの初期化 */
    initializeGraphSeries();

    /* グラフレイアウトの初期化 */
    initializeGraphLayout();

    /* windowのタイトルをファイル名に設定 */
    sheetName = table->getSheetName();
    setWindowTitle(sheetName);

    /* tableの変更に従い、グラフを再描画 */
    changedTableAction = connect(table, &TableWidget::itemChanged, this, &Graph2DSeries::updateGraphSeries);

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
            plotTableRanges.append(PlotTableRange(startRow, endRow, startCol, selectedRanges.at(rangeIndex + 1).leftColumn()));
            rangeIndex += 2;
        }
        else{  //選択範囲の列幅が2より大きいとき、プロットするxy座標の値は同じ選択範囲で連続した列で指定されている
            plotTableRanges.append(PlotTableRange(startRow, endRow, startCol, startCol + 1));
            rangeIndex += 1;
        }

        if(rangeIndex >= rangeCount) { return; } //選択範囲が終了
    }
}

void Graph2DSeries::initializeGraphLayout()
{
    setGeometry(0, 0, 620, 240);
    graph->legend()->setVisible(false);

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
    selectPageCombo->addItem("Graph");
    selectPageCombo->addItem("Series");
    selectPageCombo->addItem("Legend");
    selectPageCombo->addItem("Label");
    selectPageCombo->addItem("Axis");
    selectPageCombo->addItem("Export");
    connect(selectPageCombo, &QComboBox::activated, settingStackWidget, &QStackedWidget::setCurrentIndex);

    GraphSettingWidget *graphSettingWidget = new GraphSettingWidget(settingStackWidget, graph);
    SeriesSettingWidget *seriesSettingWidget = new SeriesSettingWidget(settingStackWidget, graph);
    LegendSettingWidget *legendSettingWidget = new LegendSettingWidget(settingStackWidget, graph);
    LabelSettingWidget *labelSettingWidget = new LabelSettingWidget(settingStackWidget, graph);
    AxisSettingWidget *axisSettingWidget = new AxisSettingWidget(settingStackWidget, graph);
    ExportSettingWidget *exportSettingWidget = new ExportSettingWidget(settingStackWidget, graph, graphView);

    settingStackWidget->addWidget(graphSettingWidget);
    settingStackWidget->addWidget(seriesSettingWidget);
    settingStackWidget->addWidget(legendSettingWidget);
    settingStackWidget->addWidget(labelSettingWidget);
    settingStackWidget->addWidget(axisSettingWidget);
    settingStackWidget->addWidget(exportSettingWidget);

    connect(seriesSettingWidget, &SeriesSettingWidget::seriesTypeChanged, this, &Graph2DSeries::changeSeriesType);
    connect(seriesSettingWidget, &SeriesSettingWidget::lineSeriesAdded, this, &Graph2DSeries::addLineSeries);
}

void Graph2DSeries::initializeGraphSeries()
{
    const qsizetype rangeCount = plotTableRanges.size();
    seriesData.resize(rangeCount);

    for(qsizetype index = 0; index < rangeCount; ++index){
        seriesData[index].rangeIndex = index;
    }

    for(const PlotTableRange& selectedRange : plotTableRanges)
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

    graph->createDefaultAxes();
}

void Graph2DSeries::updateGraphSeries(QTableWidgetItem *item)
{
    if(sheetName != table->getSheetName()) { return; }

    const int changedRow = item->row();
    const int changedCol = item->column();

    qsizetype index = 0;
    for(const SeriesData& data : seriesData)
    {
        const PlotTableRange range = plotTableRanges.at(data.rangeIndex);

        if(range.isInRange(changedRow, changedCol))
        {
            const CEnum::PlotType plotType = data.plotType;

            switch(plotType)
            {
            case CEnum::PlotType::LineSeries:
            case CEnum::PlotType::SplineSeries:
            case CEnum::PlotType::ScatterSeries:
                qobject_cast<QXYSeries*>(graph->series().at(index))->replace(changedRow - range.startRow,
                                                                             table->item(changedRow, range.colX)->text().toFloat(),
                                                                             table->item(changedRow, range.colY)->text().toFloat());
                break;
            case CEnum::PlotType::AreaSeries:

                qobject_cast<QAreaSeries*>(graph->series().at(index))->upperSeries()->replace(changedRow - range.startRow,
                                                                                              table->item(changedRow, range.colX)->text().toFloat(),
                                                                                              table->item(changedRow, range.colY)->text().toFloat());
                break;
            case CEnum::PlotType::LogressionLine:
                updateRogressionLine(index);
                break;
            default:
                break;
            }

        }
        index++;
    }
}

void Graph2DSeries::changeSeriesType(const CEnum::PlotType type, const int index)
{
    if(index != -1) { seriesData[index].plotType = type; }

    graph->removeAllSeries();

    for(qsizetype i = 0; i < seriesData.size(); ++i)
    {
        const PlotTableRange range = plotTableRanges.at(seriesData.at(i).rangeIndex);

        switch(seriesData.at(i).plotType)
        {
        case CEnum::PlotType::LineSeries:
            graph->addSeries(createXYSeries<QLineSeries>(range));
            break;
        case CEnum::PlotType::SplineSeries:
            graph->addSeries(createXYSeries<QSplineSeries>(range));
            break;
        case CEnum::PlotType::ScatterSeries:
            graph->addSeries(createXYSeries<QScatterSeries>(range));
            break;
        case CEnum::PlotType::AreaSeries:
            graph->addSeries(createAreaSeries(seriesData.at(i)));
            break;
        case CEnum::PlotType::LogressionLine:
            graph->addSeries(createRogressionLine(plotTableRanges.at(seriesData.at(i).rangeIndex)));
            break;
        default:
            break;
        }

        graph->series().at(i)->attachAxis(graph->axes(Qt::Horizontal).constLast());
        graph->series().at(i)->attachAxis(graph->axes(Qt::Vertical).constLast());
    }
}

template<class T> T* Graph2DSeries::createXYSeries(const PlotTableRange& range)
{
    T *series = new T;

    for(int row = range.startRow; row <= range.endRow; ++row)
    {
        if(table->item(row, range.colX) == nullptr || table->item(row, range.colY) == nullptr) continue;
        series->append(table->item(row, range.colX)->text().toFloat(),
                       table->item(row, range.colY)->text().toFloat());
    }

    return series;
}

QAreaSeries* Graph2DSeries::createAreaSeries(const SeriesData &data)
{
    QLineSeries *upperSeries = createXYSeries<QLineSeries>(plotTableRanges.at(data.rangeIndex));
    QAreaSeries *series = new QAreaSeries(upperSeries);

    return series;
}

QPointF Graph2DSeries::deriveRogressionLine(const PlotTableRange &range)
{
    /* x座標,y座標の合計 */
    double sumX = 0, sumY = 0;
    for(int row = range.startRow; row <= range.endRow; ++row)
    {if(table->item(row, range.colX) == nullptr || table->item(row, range.colY) == nullptr) continue;

        sumX += table->item(row, range.colX)->text().toDouble();
        sumY += table->item(row, range.colY)->text().toDouble();
    }

    /* x,yの平均 */
    const double averageX = sumX / (range.endRow - range.startRow + 1);
    const double averageY = sumY / (range.endRow - range.startRow + 1);

    /* 傾きの分子と分母 */
    double numerator = 0;
    double fraction = 0;
    for(int row = range.startRow; row <= range.endRow; ++row)
    {
        if(table->item(row, range.colX) == nullptr || table->item(row, range.colY) == nullptr) continue;
        const double x = table->item(row, range.colX)->text().toDouble();
        const double y = table->item(row, range.colY)->text().toDouble();

        numerator += (x - averageX) * (y - averageY);
        fraction += (x - averageX) * (x - averageX);
    }

    /* 傾きと切片 */
    const double slope = numerator / fraction;
    const double intercept = averageY - (slope * averageX);

    return QPointF(slope, intercept);
}

QLineSeries* Graph2DSeries::createRogressionLine(const PlotTableRange& range)
{
    QPointF cof = deriveRogressionLine(range);

    /* 直線 */
    QLineSeries *series = new QLineSeries;
    series->append(getRangeMin(Qt::Horizontal), cof.x() * getRangeMin(Qt::Horizontal) + cof.y());
    series->append(getRangeMax(Qt::Horizontal), cof.x() * getRangeMax(Qt::Horizontal) + cof.y());

    return series;
}

qreal Graph2DSeries::getRangeMin(Qt::Orientation orient)
{
    if(graph->axes(orient).size() < 1) { return NULL; }

    const QAbstractAxis::AxisType axisType = graph->axes(orient).constLast()->type();

    switch(axisType)
    {
    case QAbstractAxis::AxisTypeValue:
        return qobject_cast<QValueAxis*>(graph->axes(orient).constLast())->min();
    default :
        return NULL;
    }
}

qreal Graph2DSeries::getRangeMax(Qt::Orientation orient)
{
    if(graph->axes(orient).size() < 1) { return NULL; }

    const QAbstractAxis::AxisType axisType = graph->axes(orient).constLast()->type();

    switch(axisType)
    {
    case QAbstractAxis::AxisTypeValue:
        return qobject_cast<QValueAxis*>(graph->axes(orient).constLast())->max();
    default:
        return NULL;
    }
}

void Graph2DSeries::addLineSeries(const int index, const CEnum::PlotType type)
{
    SeriesData newSeriesData;
    newSeriesData.plotType = type;
    newSeriesData.rangeIndex = index;
    seriesData.append(newSeriesData);

    changeSeriesType(type);
}

void Graph2DSeries::updateRogressionLine(const int index)
{
    QPointF cof = deriveRogressionLine(plotTableRanges.at(seriesData.at(index).rangeIndex));

    qobject_cast<QLineSeries*>(graph->series().at(index))->replace(0,
                                                                   getRangeMin(Qt::Horizontal),
                                                                   cof.x() * getRangeMin(Qt::Horizontal) + cof.y());
    qobject_cast<QLineSeries*>(graph->series().at(index))->replace(1,
                                                                   getRangeMax(Qt::Horizontal),
                                                                   cof.x() * getRangeMax(Qt::Horizontal) + cof.y());
}











static const QStringList colorNameList()
{
    QStringList colorList;

    colorList << "color0"
              << "color1"
              << "black"
              << "white"
              << "darkGray"
              << "gray"
              << "lightGray"
              << "red"
              << "green"
              << "blue"
              << "cyan"
              << "magenta"
              << "yellow"
              << "darkRed"
              << "darkGreen"
              << "darkBlue"
              << "darkCyan"
              << "darkMagenta"
              << "darkYellow"
              << "transparent"
              << "custom";

    return colorList;
}

static const QStringList themeNameList()
{
    QStringList themeList;

    themeList << "light"
              << "blueCerulean"
              << "dark"
              << "brownSand"
              << "blueNcs"
              << "highContrast"
              << "blueIcy"
              << "qt";

    return themeList;
}

static const QStringList imgFormatList()
{
    QStringList formatList;

    for(const QByteArray& byte : QImageWriter::supportedImageFormats()){
        formatList << byte.constData();
    }

    return formatList;
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

    setEditMaximumWidth(25);
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
    setComboMaximumWidth(SETTING_EDIT_LWIDTH);

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
    setLineEditMaximumWidth(SETTING_EDIT_LWIDTH);

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
    setSpinBoxMaximumWidth(SETTING_EDIT_LWIDTH);

    connect(spinBox, &QSpinBox::valueChanged, [this](){ emit spinBoxValueChanged(spinBox->value()); });
}

void SpinBoxEditLayout::setVisible(bool visible)
{
    label->setVisible(visible);
    spinBox->setVisible(visible);
}








GraphSettingWidget::GraphSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    LineEditLayout *title = new LineEditLayout(this, "Title");
    ComboEditLayout *theme = new ComboEditLayout(this, "Theme");
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(title);
    layout->addLayout(theme);
    layout->addItem(spacer);

    theme->insertComboItems(0, themeNameList());
    theme->setComboCurrentIndex(graph->theme());

    connect(title, &LineEditLayout::lineTextEdited, graph, &QChart::setTitle);
    connect(theme, &ComboEditLayout::currentComboIndexChanged, this, &GraphSettingWidget::setTheme);
}


SeriesSettingWidget::SeriesSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    tab = new QTabWidget(this);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addWidget(tab);
    layout->addItem(spacer);

    const qsizetype num = graph->series().size();
    seriesTypeCombo.resize(num);
    lineColorCombo.resize(num);
    lineColorCustom.resize(num);

    for(qsizetype i = 0; i < num; ++i)
    {
        QWidget *tabWidget = new QWidget(tab);
        QVBoxLayout *tabWidgetLayout = new QVBoxLayout(tabWidget);
        QSpacerItem *tabSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        seriesTypeCombo[i] = new ComboEditLayout(tabWidget, "Series type");
        lineColorCombo[i] = new ComboEditLayout(tabWidget, "Line color");
        lineColorCustom[i] = new RGBEditLayout(tabWidget);
        QPushButton *addNewSeries = new QPushButton("Add series", tabWidget);

        tab->addTab(tabWidget, "series " + QString::number(i));
        tabWidget->setLayout(tabWidgetLayout);

        tabWidgetLayout->addLayout(seriesTypeCombo.at(i));
        tabWidgetLayout->addLayout(lineColorCombo.at(i));
        tabWidgetLayout->addLayout(lineColorCustom.at(i));
        tabWidgetLayout->addWidget(addNewSeries);
        tabWidgetLayout->addItem(tabSpacer);

        seriesTypeCombo.at(i)->insertComboItems(0, enumToStrings(CEnum::PlotType::LineSeries));
        lineColorCombo.at(i)->insertComboItems(0, colorNameList());
        lineColorCombo.at(i)->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
        lineColorCustom.at(i)->setColor(getLineColor(i));

        connect(seriesTypeCombo.at(i), &ComboEditLayout::currentComboIndexChanged, this, &SeriesSettingWidget::emitSeriesTypeChanged);
        connect(lineColorCombo.at(i), &ComboEditLayout::currentComboIndexChanged, this, &SeriesSettingWidget::setColorWithCombo);
        connect(lineColorCustom.at(i), &RGBEditLayout::colorEdited, this, &SeriesSettingWidget::setColorWithRGB);
        connect(addNewSeries, &QPushButton::released, this, &SeriesSettingWidget::addNewSeries);
    }
}

void SeriesSettingWidget::setColorWithCombo(const int index)
{
    const int seriesNum = tab->currentIndex();

    if(index > QT_GLOBAL_COLOR_COUNT){
        lineColorCustom.at(seriesNum)->setReadOnly(false); return;
    }

    lineColorCustom.at(seriesNum)->setReadOnly(true);
    setLineColor(Qt::GlobalColor(index));
    lineColorCustom.at(seriesNum)->setColor(index);
}

void SeriesSettingWidget::setColorWithRGB(const QColor &color)
{
    setLineColor(color);
}

void SeriesSettingWidget::setLineColor(const QColor &color)
{
    const int seriesNum = tab->currentIndex();
    const QAbstractSeries::SeriesType seriesType = graph->series().at(seriesNum)->type();

    switch (seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
        qobject_cast<QXYSeries*>(graph->series().at(seriesNum))->setColor(color); break;
    default:
        break;
    }
}

const QColor SeriesSettingWidget::getLineColor(const int index)
{
    const QAbstractSeries::SeriesType seriesType = graph->series().at(index)->type();

    switch(seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
        return qobject_cast<QXYSeries*>(graph->series().at(index))->color();
    default:
        return QColor();
    }
}

void SeriesSettingWidget::emitSeriesTypeChanged(const int type)
{
    emit seriesTypeChanged(CEnum::PlotType(type), tab->currentIndex());
}

void SeriesSettingWidget::addNewSeries()
{
    bool flagOk = false;
    const QString type = QInputDialog::getItem(this, "graph2DSeries", "select the line type", enumToStrings(CEnum::PlotType::AreaSeries), 0, false, &flagOk);

    if(!flagOk) { return; }

    emit lineSeriesAdded(tab->currentIndex(), CEnum::PlotType(getEnumIndex<CEnum::PlotType>(type)));
}


LegendSettingWidget::LegendSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QCheckBox *legendVisible = new QCheckBox("show legend", this);
    SpinBoxEditLayout *legendSize = new SpinBoxEditLayout(this, "Size");

    setLayout(layout);
    layout->addWidget(legendVisible);
    layout->addLayout(legendSize);

    legendSize->setVisible(false);
    legendSize->setSpinBoxValue(graph->legend()->font().pointSize());

    connect(legendVisible, &QCheckBox::toggled, this, &LegendSettingWidget::setLegendVisible);
    connect(legendVisible, &QCheckBox::toggled, legendSize, &SpinBoxEditLayout::setVisible);
    connect(legendSize, &SpinBoxEditLayout::spinBoxValueChanged, this, &LegendSettingWidget::setLegendPointSize);

    for(qsizetype i = 0; i < graph->series().size(); ++i)
    {
        LineEditLayout *legendName = new LineEditLayout(this, "series " + QString::number(i));
        legendName->setVisible(false);
        layout->addLayout(legendName);
        connect(legendName, &LineEditLayout::lineTextEdited, graph->series().at(i), &QAbstractSeries::setName);
        connect(legendVisible, &QCheckBox::toggled, legendName, &LineEditLayout::setVisible);
    }

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(spacer);
}

void LegendSettingWidget::setLegendPointSize(const int ps)
{
    QFont legendFont = graph->legend()->font();
    legendFont.setPointSize(ps);
    graph->legend()->setFont(legendFont);
}


LabelSettingWidget::LabelSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    tab = new QTabWidget(this);

    setLayout(layout);
    layout->addWidget(tab);

    for(qsizetype i = 0; i < graph->series().size(); ++i)
    {
        QWidget *tabWidget = new QWidget(tab);
        QVBoxLayout *tabLayout = new QVBoxLayout(tabWidget);
        QCheckBox *labelVisible = new QCheckBox("show label", tabWidget);
        QCheckBox *labelPointsVisible = new QCheckBox("show points", tabWidget);
        QCheckBox *labelPointsClipping = new QCheckBox("label clipping", tabWidget);
        QSpacerItem *tabSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        tab->addTab(tabWidget, "series " + QString::number(i));
        tabWidget->setLayout(tabLayout);
        tabLayout->addWidget(labelVisible);
        tabLayout->addWidget(labelPointsVisible);
        tabLayout->addWidget(labelPointsClipping);
        tabLayout->addItem(tabSpacer);

        labelPointsClipping->setChecked(true);

        connect(labelVisible, &QCheckBox::toggled, this, &LabelSettingWidget::setPointsVisible);
        connect(labelPointsVisible, &QCheckBox::toggled, this, &LabelSettingWidget::setPointLabelsVisible);
        connect(labelPointsClipping, &QCheckBox::toggled, this, &LabelSettingWidget::setPointLabelsClipping);
    }
}

void LabelSettingWidget::setPointsVisible(const bool visible)
{
    const int index = tab->currentIndex();
    const QAbstractSeries::SeriesType seriesType = graph->series().at(index)->type();

    switch(seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
        qobject_cast<QXYSeries*>(graph->series().at(index))->setPointsVisible(visible);
        break;
    default:
        break;
    }
}

void LabelSettingWidget::setPointLabelsVisible(const bool visible)
{
    const int index = tab->currentIndex();
    const QAbstractSeries::SeriesType seriesType = graph->series().at(index)->type();

    switch(seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
        qobject_cast<QXYSeries*>(graph->series().at(index))->setPointLabelsVisible(visible); break;
    default:
        break;
    }
}

void LabelSettingWidget::setPointLabelsClipping(const bool clipping)
{
    const int index = tab->currentIndex();
    const QAbstractSeries::SeriesType seriesType = graph->series().at(index)->type();

    switch(seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
        qobject_cast<QXYSeries*>(graph->series().at(index))->setPointLabelsClipping(clipping); break;
    default:
        break;
    }
}


ExportSettingWidget::ExportSettingWidget(QWidget *parent, QChart *graph, QChartView *graphView)
    : QWidget(parent), graph(graph), graphView(graphView)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    fileName = new LineEditLayout(this, "File name");
    type = new ComboEditLayout(this, "Export to");
    QPushButton *button = new QPushButton("Export", this);
    QStackedWidget *stackWidget = new QStackedWidget(this);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(fileName);
    layout->addLayout(type);
    layout->addWidget(button);
    layout->addWidget(stackWidget);
    layout->addItem(spacer);

    type->insertComboItems(0, QStringList() << "Image");

    connect(type, &ComboEditLayout::currentComboIndexChanged, stackWidget, &QStackedWidget::setCurrentIndex);
    connect(button, &QPushButton::released, this, &ExportSettingWidget::exportFile);

    QGroupBox *exportImageBox = new QGroupBox("Image", stackWidget);
    QVBoxLayout *exportImageLayout = new QVBoxLayout(exportImageBox);
    imageFormat = new ComboEditLayout(exportImageBox, "Format");
    QSpacerItem *exportImageSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    stackWidget->addWidget(exportImageBox);
    exportImageBox->setLayout(exportImageLayout);
    exportImageLayout->addLayout(imageFormat);
    exportImageLayout->addItem(exportImageSpacer);

    imageFormat->insertComboItems(0, imgFormatList());

}

void ExportSettingWidget::exportFile()
{
    switch(type->currentComboIndex())
    {
    case 0:
        exportToImage(); break;
    default:
        break;
    }
}

void ExportSettingWidget::exportToImage()
{
    const QString directoryPath = QFileDialog::getExistingDirectory(this);
    const QString exportFileName = fileName->lineEditText();
    const QString format = imageFormat->currentComboText();

    if(directoryPath.isEmpty()) { return; }

    QImage img = graphView->grab().toImage();
    const bool success = img.save(directoryPath + "/" + exportFileName + "." + format,
                                  format.toLocal8Bit().constData());
    if(success) { return; }

    QMessageBox::critical(this, "Error", "Failed to save.");
}


AxisSettingGroupBox::AxisSettingGroupBox(QWidget *parent, QChart *graph, const QString& text, const Qt::Orientation orient)
    : QGroupBox(text, parent), graph(graph), orient(orient)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    rangeMin = new LineEditLayout(this, "Min");
    rangeMax = new LineEditLayout(this, "Max");
    QCheckBox *axisNameVisible = new QCheckBox("Show axis name", this);
    LineEditLayout *axisName = new LineEditLayout(this, "Axis name");
    axisNameSize = new SpinBoxEditLayout(this, "Axis name size");
    QCheckBox *axisLabelVisible = new QCheckBox("Show label", this);
    labelAngle = new LineEditLayout(this, "Label angle");
    labelColor = new ComboEditLayout(this, "Label color");
    labelColorCustom = new RGBEditLayout(this);
    labelSize = new SpinBoxEditLayout(this, "Label size");
    QCheckBox *gridVisible = new QCheckBox("Show grid", this);
    gridColor = new ComboEditLayout(this, "Grid color");
    gridColorCustom = new RGBEditLayout(this);

    setLayout(layout);
    layout->addLayout(rangeMin);
    layout->addLayout(rangeMax);
    layout->addWidget(axisNameVisible);
    layout->addLayout(axisName);
    layout->addLayout(axisNameSize);
    layout->addWidget(axisLabelVisible);
    layout->addLayout(labelAngle);
    layout->addLayout(labelColor);
    layout->addLayout(labelColorCustom);
    layout->addLayout(labelSize);
    layout->addWidget(gridVisible);
    layout->addLayout(gridColor);
    layout->addLayout(gridColorCustom);

    rangeMin->setLineEditMaximumWidth(SETTING_EDIT_SWIDTH);
    rangeMax->setLineEditMaximumWidth(SETTING_EDIT_SWIDTH);
    axisNameVisible->setChecked(true);
    axisLabelVisible->setChecked(true);
    labelColor->insertComboItems(0, colorNameList());
    labelColor->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    gridVisible->setChecked(true);
    gridColor->insertComboItems(0, colorNameList());
    gridColor->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);

    if(graph->axes(orient).size() < 1) { return; }

    setRangeEdit();
    labelColorCustom->setColor(graph->axes(orient).constLast()->labelsColor());
    gridColorCustom->setColor(graph->axes(orient).constLast()->gridLineColor());
    axisNameSize->setSpinBoxValue(graph->axes(orient).constLast()->titleFont().pointSize());
    labelAngle->setLineEditText(QString::number(graph->axes(orient).constLast()->labelsAngle()));
    labelSize->setSpinBoxValue(graph->axes(orient).constLast()->labelsFont().pointSize());
    labelColorCustom->setColor(graph->axes(orient).constLast()->labelsColor());
    gridColorCustom->setColor(graph->axes(orient).constLast()->gridLineColor());

    connect(rangeMin, &LineEditLayout::lineTextEdited, graph->axes(orient).constLast(), &QAbstractAxis::setMin);
    connect(rangeMax, &LineEditLayout::lineTextEdited, graph->axes(orient).constLast(), &QAbstractAxis::setMax);
    connectRangeEdit();
    connect(axisNameVisible, &QCheckBox::toggled, graph->axes(orient).constLast(), &QAbstractAxis::setTitleVisible);
    connect(axisNameVisible, &QCheckBox::toggled, axisName, &LineEditLayout::setVisible);
    connect(axisNameVisible, &QCheckBox::toggled, axisNameSize, &SpinBoxEditLayout::setVisible);
    connect(axisName, &LineEditLayout::lineTextEdited, graph->axes(orient).constLast(), &QAbstractAxis::setTitleText);
    connect(axisNameSize, &SpinBoxEditLayout::spinBoxValueChanged, this, &AxisSettingGroupBox::setAxisNameSize);
    connect(axisLabelVisible, &QCheckBox::toggled, graph->axes(orient).constLast(), &QAbstractAxis::setLabelsVisible);
    connect(axisLabelVisible, &QCheckBox::toggled, labelAngle, &LineEditLayout::setVisible);
    connect(axisLabelVisible, &QCheckBox::toggled, labelColor, &ComboEditLayout::setVisible);
    connect(axisLabelVisible, &QCheckBox::toggled, labelColorCustom, &RGBEditLayout::setVisible);
    connect(axisLabelVisible, &QCheckBox::toggled, labelSize, &SpinBoxEditLayout::setVisible);
    connect(labelAngle, &LineEditLayout::lineTextEdited, this, &AxisSettingGroupBox::setAxisLabelAngle);
    connect(labelColor, &ComboEditLayout::currentComboIndexChanged, this, &AxisSettingGroupBox::setAxisLabelColor);
    connect(labelColorCustom, &RGBEditLayout::colorEdited, graph->axes(orient).constLast(), &QAbstractAxis::setLabelsColor);
    connect(labelSize, &SpinBoxEditLayout::spinBoxValueChanged, this, &AxisSettingGroupBox::setAxisLabelSize);
    connect(gridVisible, &QCheckBox::toggled, graph->axes(orient).constLast(), &QAbstractAxis::setGridLineVisible);
    connect(gridVisible, &QCheckBox::toggled, gridColor, &ComboEditLayout::setVisible);
    connect(gridVisible, &QCheckBox::toggled, gridColorCustom, &RGBEditLayout::setVisible);
    connect(gridColor, &ComboEditLayout::currentComboIndexChanged, this, &AxisSettingGroupBox::setGridColor);
    connect(gridColorCustom, &RGBEditLayout::colorEdited, graph->axes(orient).constLast(), &QAbstractAxis::setGridLineColor);
}

void AxisSettingGroupBox::connectRangeEdit()
{
    disconnect(rangeMinEditConnection);
    disconnect(rangeMaxEditConnection);

    const QAbstractAxis::AxisType axisType = graph->axes(orient).constLast()->type();

    switch(axisType)
    {
    case QAbstractAxis::AxisTypeValue:
        rangeMinEditConnection = connect(qobject_cast<QValueAxis*>(graph->axes(orient).constLast()), &QValueAxis::minChanged, this, &AxisSettingGroupBox::setMinEdit);
        rangeMaxEditConnection = connect(qobject_cast<QValueAxis*>(graph->axes(orient).constLast()), &QValueAxis::minChanged, this, &AxisSettingGroupBox::setMinEdit);
    default:
        break;
    }
}

void AxisSettingGroupBox::setAxisNameSize(const int ps)
{
    QFont axisNameFont = graph->axes(orient).constLast()->titleFont();
    axisNameFont.setPointSize(ps);
    graph->axes(orient).constLast()->setTitleFont(axisNameFont);
}

void AxisSettingGroupBox::setAxisLabelAngle(const QString& angle)
{
    graph->axes(orient).constLast()->setLabelsAngle(angle.toInt());
}

void AxisSettingGroupBox::setAxisLabelColor(const int index)
{
    if(index > QT_GLOBAL_COLOR_COUNT){
        labelColorCustom->setReadOnly(false); return;
    }

    labelColorCustom->setReadOnly(true);
    labelColorCustom->setColor(index);
    graph->axes(orient).constLast()->setLabelsColor(Qt::GlobalColor(index));
}

void AxisSettingGroupBox::setAxisLabelSize(const int ps)
{
    QFont labelsFont = graph->axes(orient).constLast()->labelsFont();
    labelsFont.setPointSize(ps);
    graph->axes(orient).constLast()->setLabelsFont(labelsFont);
}

void AxisSettingGroupBox::setGridColor(const int index)
{
    if(index > QT_GLOBAL_COLOR_COUNT){
        gridColorCustom->setReadOnly(false); return;
    }

    gridColorCustom->setReadOnly(true);
    gridColorCustom->setColor(index);
    graph->axes(orient).constLast()->setGridLineColor(Qt::GlobalColor(index));
}

void AxisSettingGroupBox::setRangeEdit()
{
    const QAbstractAxis::AxisType axisType = graph->axes(orient).constLast()->type();

    switch(axisType)
    {
    case QAbstractAxis::AxisTypeValue:
        setMinEdit(qobject_cast<QValueAxis*>(graph->axes(orient).constLast())->min());
        setMaxEdit(qobject_cast<QValueAxis*>(graph->axes(orient).constLast())->max());
        break;
    default:
        break;
    }
}


AxisSettingWidget::AxisSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    xAxisGroupBox = new AxisSettingGroupBox(this, graph, "X Axis", Qt::Horizontal);
    yAxisGroupBox = new AxisSettingGroupBox(this, graph, "Y Axis", Qt::Vertical);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addWidget(xAxisGroupBox);
    layout->addWidget(yAxisGroupBox);
    layout->addItem(spacer);
}












