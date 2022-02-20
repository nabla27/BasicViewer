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
 *
 *
 * QGraphicsItem ---|--- QAbstractGraphicsShapeItem ---|--- QGraphicsEllipseItem
 *                  |                                  |--- QGraphicsPathItem
 *                  |                                  |--- QGraphicsPolygonItem
 *                  |                                  |--- QGraphicsRectItem
 *                  |                                  |--- QGraphicsSimpleItem
 *                  |
 *                  |--- QGraphicsObject --------------|--- QGraphicsSvgItem
 *                  |                                  |--- QGraphicsTextItem
 *                  |                                  |--- QGraphicsWidget
 *                  |
 *                  |
 *
 *
 *
 *
 * [ tableWidgetの変更が変更されたとき ]
 * TableWidget::itemChanged()  ---->  Graph2DSeries::updateGraphSeries()  ---->  replace coordinate
 *
 * [ グラフのタイプがcomboで変更されたとき ]
 * SeriesSettingWidget::seriesTypeChanged()  ---->  SeriesSettingWidget::changeWidgetItemVisible()
 *                                           ---->  Graph2DSeries::changeSeriesType()  ---->  removeAllSeries()  ---->  Graph2DSeries::addSeriesToGraph()
 *                                           ---->  LegendSettingWidget::reconnectSeriesNameEditor()
 *
 * [ グラフにseriesを追加するボタンが押されたとき ]
 * SeriesSettingWidget::addNewSeries()  ---->  SeriesSettingWidget::newSeriesAdded()  ---->  SeriesSettingWidget::addTab()
 *                                                                                    ---->  Graph2DSeries::addNewSeries()  ---->  Graph2DSeries::addSeriesToGraph()
 *                                                                                    ---->  LabelSettingWidget::addTab()
 *                                                                                    ---->  LegendSettingWidget::addSeriesNameEdit()
 *
 * [ グラフ上のマウス移動 ]
 * ChartView::mouseMoveEvent()  ---->  GraphSettingWidget::setCoordinateValue()
 */


ChartView::ChartView(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
{
    itemMenu = new QMenu(this);

    QAction *addText = new QAction("text", itemMenu);
    itemMenu->addAction(addText);
    connect(addText, &QAction::triggered, this, &ChartView::addTextItem);

    QAction*addLine = new QAction("line", itemMenu);
    itemMenu->addAction(addLine);
    connect(addLine, &QAction::triggered, this, &ChartView::addLineItem);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint cursorPos = event->pos();
    const QPointF coordinate = chart()->mapToValue(cursorPos);
    emit mouseCoordinateMoved(QString::number(coordinate.x()), QString::number(coordinate.y()));

    if(event->buttons() == Qt::RightButton)
        moveGraph(cursorPos);

    QChartView::mouseMoveEvent(event);
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if(event->buttons() != Qt::LeftButton)
    {
        const int degree = event->angleDelta().y();
        if(degree >= 0)
            chart()->zoom(1 + 0.1);
        else
            chart()->zoom(1 - 0.1);
    }

    QChartView::wheelEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        dragStartPoint.pos = event->pos();

        const QAbstractAxis::AxisType typeHorizontal = chart()->axes(Qt::Horizontal).constLast()->type();
        const QAbstractAxis::AxisType typeVertical = chart()->axes(Qt::Vertical).constLast()->type();

        switch (typeHorizontal) {
        case QAbstractAxis::AxisTypeValue:{
            const QValueAxis *axis = qobject_cast<QValueAxis*>(chart()->axes(Qt::Horizontal).constLast());
            dragStartPoint.min.setX(axis->min());
            dragStartPoint.max.setX(axis->max());
            dragStartPoint.pixelWidth.setX((axis->max() - axis->min()) / chart()->plotArea().width());
            break;
        }
        default:
            break;
        }

        switch(typeVertical) {
        case QAbstractAxis::AxisTypeValue:{
            const QValueAxis *axis = qobject_cast<QValueAxis*>(chart()->axes(Qt::Vertical).constLast());
            dragStartPoint.min.setY(axis->min());
            dragStartPoint.max.setY(axis->max());
            dragStartPoint.pixelWidth.setY((axis->max() - axis->min()) / chart()->plotArea().height());
            break;
        }
        default:
                break;
        }
    }

    QChartView::mousePressEvent(event);
}

void ChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    itemMenu->exec(viewport()->mapToGlobal(event->pos()));

    QChartView::mouseDoubleClickEvent(event);
}

void ChartView::moveGraph(const QPoint& cursorPos)
{
    QAbstractAxis *const axisX = chart()->axes(Qt::Horizontal).constLast();
    const qreal horizontalMove = dragStartPoint.pixelWidth.x() * (cursorPos.x() - dragStartPoint.pos.x());
    axisX->setMin(dragStartPoint.min.x() - horizontalMove);
    axisX->setMax(dragStartPoint.max.x() - horizontalMove);

    QAbstractAxis *const axisY = chart()->axes(Qt::Vertical).constLast();
    const qreal verticalMove = dragStartPoint.pixelWidth.y() * (cursorPos.y() - dragStartPoint.pos.y());
    axisY->setMin(dragStartPoint.min.y() + verticalMove);
    axisY->setMax(dragStartPoint.max.y() + verticalMove);
}

void ChartView::addTextItem()
{
    GraphicsTextItem *textItem = new GraphicsTextItem("text", chart());
    textItem->setPos(mapFromGlobal(cursor().pos()));  //cursor().pos()はグローバル座標を返す。setPos()はChartViewでの座標で指定。
}

void ChartView::addLineItem()
{
    GraphicsLineItem *lineItem = new GraphicsLineItem(QLineF(-25, -25, 25, 25), chart());
    QPen linePen = lineItem->pen();
    linePen.setWidth(2);
    lineItem->setPen(linePen);
    lineItem->setPos(mapFromGlobal(cursor().pos()));
}






GraphicsTextItem::GraphicsTextItem(const QString& text, QGraphicsItem *parent)
    : QGraphicsSimpleTextItem(text, parent)
{
    setAcceptHoverEvents(true);
}

ItemSettingWidget* GraphicsTextItem::settingWidget;

void GraphicsTextItem::setSettingWidget(ItemSettingWidget *widget) { settingWidget = widget; }

void GraphicsTextItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if(event->delta() > 0)
        setRotation(rotation() - 2);
    else
        setRotation(rotation() + 2);
}

void GraphicsTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
}

void GraphicsTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
}

void GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
    settingWidget->setItemSettingWidget(this);
}

void GraphicsTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
}

void GraphicsTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    settingWidget->setItemSettingWidget(this);
}



GraphicsLineItem::GraphicsLineItem(const QLineF& line, QGraphicsItem *parent)
    : QGraphicsLineItem(line, parent)
{
    setAcceptHoverEvents(true);
}

ItemSettingWidget* GraphicsLineItem::settingWidget;

void GraphicsLineItem::setSettingWidget(ItemSettingWidget *widget) { settingWidget = widget; }

void GraphicsLineItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
#if 0
    double theta = (2.0 / 180.0) * 3.1415; qDebug() << theta;
    if(event->delta() < 0) { theta = -theta; }

    const QPointF previousP1 = line().p1();
    const QPointF previousP2 = line().p2();

    setLine(QLineF(QPointF(previousP1.x() * cos(theta) - previousP1.y() * sin(theta), previousP1.y() * sin(theta) + previousP1.y() * cos(theta)),
                   QPointF(previousP2.x() * cos(theta) - previousP2.y() * sin(theta), previousP2.y() * sin(theta) + previousP2.y() * cos(theta))));
#endif
}

void GraphicsLineItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::OpenHandCursor);
}

void GraphicsLineItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
}

void GraphicsLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
    settingWidget->setItemSettingWidget(this);
}

void GraphicsLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    setPos(mapToParent(event->pos() - event->buttonDownPos(Qt::LeftButton)));
}

void GraphicsLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    settingWidget->setItemSettingWidget(this);
}














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
    setGeometry(0, 0, 650, 240);
    graph->legend()->setVisible(false);

    /* レイアウトのグラフ部分 */
    graphView = new ChartView(graph);   //graphViewのウィンドウサイズは4:3がいい感じ

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
    selectPageCombo->addItem("Item");
    connect(selectPageCombo, &QComboBox::activated, settingStackWidget, &QStackedWidget::setCurrentIndex);

    GraphSettingWidget *graphSettingWidget = new GraphSettingWidget(settingStackWidget, graph);
    SeriesSettingWidget *seriesSettingWidget = new SeriesSettingWidget(settingStackWidget, graph);
    LegendSettingWidget *legendSettingWidget = new LegendSettingWidget(settingStackWidget, graph);
    LabelSettingWidget *labelSettingWidget = new LabelSettingWidget(settingStackWidget, graph);
    AxisSettingWidget *axisSettingWidget = new AxisSettingWidget(settingStackWidget, graph);
    ExportSettingWidget *exportSettingWidget = new ExportSettingWidget(settingStackWidget, graph, graphView);
    ItemSettingWidget *itemSettingWidget = new ItemSettingWidget(settingStackWidget);

    settingStackWidget->addWidget(graphSettingWidget);
    settingStackWidget->addWidget(seriesSettingWidget);
    settingStackWidget->addWidget(legendSettingWidget);
    settingStackWidget->addWidget(labelSettingWidget);
    settingStackWidget->addWidget(axisSettingWidget);
    settingStackWidget->addWidget(exportSettingWidget);
    settingStackWidget->addWidget(itemSettingWidget);

    connect(seriesSettingWidget, &SeriesSettingWidget::seriesTypeChanged, this, &Graph2DSeries::changeSeriesType);
    connect(seriesSettingWidget, &SeriesSettingWidget::seriesTypeChanged, legendSettingWidget, &LegendSettingWidget::reconnectSeriesNameEditor);
    connect(seriesSettingWidget, &SeriesSettingWidget::newSeriesAdded, this, &Graph2DSeries::addNewSeries);
    connect(seriesSettingWidget, &SeriesSettingWidget::newSeriesAdded, labelSettingWidget, &LabelSettingWidget::addTab);
    connect(seriesSettingWidget, &SeriesSettingWidget::newSeriesAdded, legendSettingWidget, &LegendSettingWidget::addSeriesNameEditer);
    connect(graphView, &ChartView::mouseCoordinateMoved, graphSettingWidget, &GraphSettingWidget::setCoordinateValue);

    GraphicsTextItem::setSettingWidget(itemSettingWidget);
    GraphicsLineItem::setSettingWidget(itemSettingWidget);
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
            const ChartEnum::PlotType plotType = data.plotType;

            switch(plotType)
            {
            case ChartEnum::PlotType::LineSeries:
            case ChartEnum::PlotType::SplineSeries:
            case ChartEnum::PlotType::ScatterSeries:
                qobject_cast<QXYSeries*>(graph->series().at(index))->replace(changedRow - range.startRow,
                                                                             table->item(changedRow, range.colX)->text().toFloat(),
                                                                             table->item(changedRow, range.colY)->text().toFloat());
                break;
            case ChartEnum::PlotType::AreaSeries:

                qobject_cast<QAreaSeries*>(graph->series().at(index))->upperSeries()->replace(changedRow - range.startRow,
                                                                                              table->item(changedRow, range.colX)->text().toFloat(),
                                                                                              table->item(changedRow, range.colY)->text().toFloat());
                break;
            case ChartEnum::PlotType::LogressionLine:
                updateRogressionLine(index);
                break;
            default:
                break;
            }

        }
        index++;
    }
}

void Graph2DSeries::changeSeriesType(const ChartEnum::PlotType type, const int index)
{
    if(index != -1) { seriesData[index].plotType = type; }

    graph->removeAllSeries();

    const int seriesCount = seriesData.size();

    for(qsizetype i = 0; i < seriesCount; ++i)
        addSeriesToGraph(seriesData.at(i));
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
    const QString slope = (cof.x() >= 0) ? QString::number(cof.x()) : "- " + QString::number(abs(cof.x()));
    const QString intercept = (cof.y() >= 0) ? " + " + QString::number(cof.y()) : " - " + QString::number(abs(cof.y()));
    series->setName("Y = " + slope + "X" + intercept);

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

void Graph2DSeries::addNewSeries(const int index, const ChartEnum::PlotType type)
{
    SeriesData newSeriesData;
    newSeriesData.plotType = type;
    newSeriesData.rangeIndex = seriesData.at(index).rangeIndex;
    seriesData.append(newSeriesData);

    addSeriesToGraph(newSeriesData);
}

void Graph2DSeries::addSeriesToGraph(const SeriesData& data)
{
    const ChartEnum::PlotType type = data.plotType;
    const PlotTableRange range = plotTableRanges.at(data.rangeIndex);

    switch(type)
    {
    case ChartEnum::PlotType::LineSeries:
        graph->addSeries(createXYSeries<QLineSeries>(range)); break;
    case ChartEnum::PlotType::SplineSeries:
        graph->addSeries(createXYSeries<QSplineSeries>(range)); break;
    case ChartEnum::PlotType::ScatterSeries:
        graph->addSeries(createXYSeries<QScatterSeries>(range)); break;
    case ChartEnum::PlotType::AreaSeries:
        graph->addSeries(createAreaSeries(data)); break;
    case ChartEnum::PlotType::LogressionLine:
        graph->addSeries(createRogressionLine(range)); break;
    default:
        return;
        break;
    }

    graph->series().constLast()->attachAxis(graph->axes(Qt::Horizontal).constLast());
    graph->series().constLast()->attachAxis(graph->axes(Qt::Vertical).constLast());
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

    const QString slope = (cof.x() >= 0) ? QString::number(cof.x()) : "- " + QString::number(abs(cof.x()));
    const QString intercept = (cof.y() >= 0) ? " + " + QString::number(cof.y()) : " - " + QString::number(abs(cof.y()));
    graph->series().at(index)->setName("Y = " + slope + "X" + intercept);
}































GraphSettingWidget::GraphSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    xCoordinate = new LineEditLayout(this, "X :");
    yCoordinate = new LineEditLayout(this, "Y :");
    LineEditLayout *title = new LineEditLayout(this, "Title");
    SpinBoxEditLayout *titleSize = new SpinBoxEditLayout(this, "Title size");
    ComboEditLayout *theme = new ComboEditLayout(this, "Theme");
    BlankSpaceLayout *blankSpace = new BlankSpaceLayout(250, 0);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(xCoordinate);
    layout->addLayout(yCoordinate);
    layout->addLayout(title);
    layout->addLayout(titleSize);
    layout->addLayout(theme);
    layout->addLayout(blankSpace);
    layout->addItem(spacer);

    xCoordinate->setReadOnly(true);
    yCoordinate->setReadOnly(true);
    titleSize->setSpinBoxValue(graph->titleFont().pointSize());
    theme->insertComboItems(0, enumToStrings(ChartEnum::Theme(0)));
    theme->setComboCurrentIndex(graph->theme());

    connect(title, &LineEditLayout::lineTextEdited, graph, &QChart::setTitle);
    connect(titleSize, &SpinBoxEditLayout::spinBoxValueChanged, this, &GraphSettingWidget::setTitleSize);
    connect(theme, &ComboEditLayout::currentComboIndexChanged, this, &GraphSettingWidget::setTheme);
}

void GraphSettingWidget::setTitleSize(const int ps)
{
    QFont titleFont = graph->titleFont();
    titleFont.setPointSize(ps);
    graph->setTitleFont(titleFont);
}

void GraphSettingWidget::setCoordinateValue(const QString& x, const QString& y)
{
    xCoordinate->setLineEditText(x);
    yCoordinate->setLineEditText(y);
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

    const qsizetype seriesCount = graph->series().size();

    for(qsizetype i = 0; i < seriesCount; ++i)
        addTab(ChartEnum::PlotType::LineSeries);

    connect(this, &SeriesSettingWidget::seriesTypeChanged, this, &SeriesSettingWidget::changeWidgetItemVisible);
}

void SeriesSettingWidget::setLineVisible(const bool visible)
{
    const int seriesIndex = tab->currentIndex();
    graph->series().at(seriesIndex)->setVisible(visible);
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
    const int seriesIndex = tab->currentIndex();
    const QAbstractSeries::SeriesType seriesType = graph->series().at(seriesIndex)->type();

    switch (seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
    case QAbstractSeries::SeriesTypeScatter:
        qobject_cast<QXYSeries*>(graph->series().at(seriesIndex))->setColor(color); break;
    case QAbstractSeries::SeriesTypeArea:
        qobject_cast<QAreaSeries*>(graph->series().at(seriesIndex))->setColor(color); break;
    default:
        break;
    }
}

const QColor SeriesSettingWidget::getLineColor(const int index) const
{
    const QAbstractSeries::SeriesType seriesType = graph->series().at(index)->type();

    switch(seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
    case QAbstractSeries::SeriesTypeScatter:
        return qobject_cast<QXYSeries*>(graph->series().at(index))->color();
    case QAbstractSeries::SeriesTypeArea:
        return qobject_cast<QAreaSeries*>(graph->series().at(index))->color();
    default:
        return QColor();
    }
}

void SeriesSettingWidget::emitSeriesTypeChanged(const int type)
{
    emit seriesTypeChanged(ChartEnum::PlotType(type), tab->currentIndex());
}

void SeriesSettingWidget::setScatterType(const int type)
{
    const int seriesIndex = tab->currentIndex();
    qobject_cast<QScatterSeries*>(graph->series().at(seriesIndex))->setMarkerShape(QScatterSeries::MarkerShape(type));
}

void SeriesSettingWidget::addNewSeries()
{
    bool flagOk = false;
    const QString type = QInputDialog::getItem(this, "graph2DSeries", "select the line type", enumToStrings(ChartEnum::PlotType::AreaSeries), 0, false, &flagOk);

    if(!flagOk) { return; }

    emit newSeriesAdded(tab->currentIndex(), ChartEnum::PlotType(getEnumIndex<ChartEnum::PlotType>(type)));
    addTab(ChartEnum::PlotType(getEnumIndex<ChartEnum::PlotType>(type)));
}

void SeriesSettingWidget::addTab(ChartEnum::PlotType type)
{
    QWidget *tabWidget = new QWidget(tab);
    QVBoxLayout *tabWidgetLayout = new QVBoxLayout(tabWidget);
    QSpacerItem *tabSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QCheckBox *lineVisible = new QCheckBox("Visible", tabWidget);
    seriesTypeCombo.append(new ComboEditLayout(tabWidget, "Series type"));
    lineColorCombo.append(new ComboEditLayout(tabWidget, "Color"));
    lineColorCustom.append(new RGBEditLayout(tabWidget));
    scatterTypeCombo.append(new ComboEditLayout(tabWidget, "Type"));
    PushButtonLayout *addNewSeries = new PushButtonLayout(tabWidget, "Add series");

    tab->addTab(tabWidget, "series " + QString::number(tab->count()));
    tabWidget->setLayout(tabWidgetLayout);

    tabWidgetLayout->addWidget(lineVisible);
    tabWidgetLayout->addLayout(seriesTypeCombo.constLast());
    tabWidgetLayout->addLayout(lineColorCombo.constLast());
    tabWidgetLayout->addLayout(lineColorCustom.constLast());
    tabWidgetLayout->addLayout(scatterTypeCombo.constLast());
    tabWidgetLayout->addLayout(new BlankSpaceLayout(0, 30));
    tabWidgetLayout->addLayout(addNewSeries);
    tabWidgetLayout->addItem(tabSpacer);

    lineVisible->setChecked(true);
    seriesTypeCombo.constLast()->insertComboItems(0, enumToStrings(ChartEnum::PlotType(0)));
    seriesTypeCombo.constLast()->setComboCurrentIndex((int)type);
    lineColorCombo.constLast()->insertComboItems(0, colorNameList());
    lineColorCombo.constLast()->setComboCurrentIndex(QT_GLOBAL_COLOR_COUNT + 1);
    lineColorCustom.constLast()->setColor(getLineColor(tab->count() - 1));
    scatterTypeCombo.constLast()->insertComboItems(0, enumToStrings(ChartEnum::MarkerShape(0)));
    scatterTypeCombo.constLast()->setVisible(type == ChartEnum::PlotType::ScatterSeries);
    addNewSeries->setButtonMinimumWidth(SETTING_EDIT_LWIDTH);

    connect(lineVisible, &QCheckBox::toggled, this, &SeriesSettingWidget::setLineVisible);
    connect(seriesTypeCombo.constLast(), &ComboEditLayout::currentComboIndexChanged, this, &SeriesSettingWidget::emitSeriesTypeChanged);
    connect(lineColorCombo.constLast(), &ComboEditLayout::currentComboIndexChanged, this, &SeriesSettingWidget::setColorWithCombo);
    connect(lineColorCustom.constLast(), &RGBEditLayout::colorEdited, this, &SeriesSettingWidget::setColorWithRGB);
    connect(scatterTypeCombo.constLast(), &ComboEditLayout::currentComboIndexChanged, this, &SeriesSettingWidget::setScatterType);
    connect(addNewSeries, &PushButtonLayout::buttonReleased, this, &SeriesSettingWidget::addNewSeries);
}

void SeriesSettingWidget::changeWidgetItemVisible(const ChartEnum::PlotType type, const int index)
{
    switch(type)
    {
    case ChartEnum::PlotType::LineSeries:
    case ChartEnum::PlotType::SplineSeries:
    case ChartEnum::PlotType::AreaSeries:
    case ChartEnum::PlotType::LogressionLine:
        scatterTypeCombo.at(index)->setVisible(false); break;
    case ChartEnum::PlotType::ScatterSeries:
        scatterTypeCombo.at(index)->setVisible(true); break;
    default:
        break;
    }
}


LegendSettingWidget::LegendSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    layout = new QVBoxLayout(this);
    legendVisible = new QCheckBox("show legend", this);
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
        addSeriesNameEditer();

    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(spacer);
}

void LegendSettingWidget::setLegendPointSize(const int ps)
{
    QFont legendFont = graph->legend()->font();
    legendFont.setPointSize(ps);
    graph->legend()->setFont(legendFont);
}

void LegendSettingWidget::reconnectSeriesNameEditor()
{
    const int seriesCount = graph->series().size();

    for(int i = 0; i < seriesCount; ++i)
        connect(legendName.at(i), &LineEditLayout::lineTextEdited, graph->series().at(i), &QAbstractSeries::setName);
}

void LegendSettingWidget::addSeriesNameEditer()
{
    const qsizetype seriesCount = graph->series().size();

    legendName.append(new LineEditLayout(this, "series " + QString::number(seriesCount - 1)));
    legendName.constLast()->setVisible(legendVisible->isChecked());
    layout->insertLayout(2 + seriesCount - 1, legendName.constLast());
    connect(legendName.constLast(), &LineEditLayout::lineTextEdited, graph->series().at(seriesCount - 1), &QAbstractSeries::setName);
    connect(legendVisible, &QCheckBox::toggled, legendName.constLast(), &LineEditLayout::setVisible);
}


LabelSettingWidget::LabelSettingWidget(QWidget *parent, QChart *graph)
    : QWidget(parent), graph(graph)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    tab = new QTabWidget(this);

    setLayout(layout);
    layout->addWidget(tab);

    const int seriesCount = graph->series().size();
    for(qsizetype i = 0; i < seriesCount; ++i)
        addTab();
}

void LabelSettingWidget::setPointsVisible(const bool visible)
{
    const int index = tab->currentIndex();
    const QAbstractSeries::SeriesType seriesType = graph->series().at(index)->type();

    switch(seriesType)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
    case QAbstractSeries::SeriesTypeScatter:
        qobject_cast<QXYSeries*>(graph->series().at(index))->setPointsVisible(visible); break;
    case QAbstractSeries::SeriesTypeArea:
        qobject_cast<QAreaSeries*>(graph->series().at(index))->setPointsVisible(visible); break;
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
    case QAbstractSeries::SeriesTypeScatter:
        qobject_cast<QXYSeries*>(graph->series().at(index))->setPointLabelsVisible(visible); break;
    case QAbstractSeries::SeriesTypeArea:
        qobject_cast<QAreaSeries*>(graph->series().at(index))->setPointLabelsVisible(visible); break;
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
    case QAbstractSeries::SeriesTypeScatter:
        qobject_cast<QXYSeries*>(graph->series().at(index))->setPointLabelsClipping(clipping); break;
    case QAbstractSeries::SeriesTypeArea:
        qobject_cast<QAreaSeries*>(graph->series().at(index))->setPointLabelsClipping(clipping); break;
    default:
        break;
    }
}

void LabelSettingWidget::setPointsSize(const QString& ps)
{
    const int seriesIndex = tab->currentIndex();
    const QAbstractSeries::SeriesType type = graph->series().at(seriesIndex)->type();

    switch(type)
    {
    case QAbstractSeries::SeriesTypeLine:
    case QAbstractSeries::SeriesTypeSpline:
    case QAbstractSeries::SeriesTypeScatter:
        qobject_cast<QXYSeries*>(graph->series().at(seriesIndex))->setMarkerSize(ps.toDouble());
    default:
        break;
    }
}

void LabelSettingWidget::addTab()
{
    QWidget *tabWidget = new QWidget(tab);
    QVBoxLayout *tabLayout = new QVBoxLayout(tabWidget);
    QCheckBox *labelVisible = new QCheckBox("Show label", tabWidget);
    QCheckBox *labelPointsVisible = new QCheckBox("Show points", tabWidget);
    QCheckBox *labelPointsClipping = new QCheckBox("Label clipping", tabWidget);
    labelPointsSize.append(new LineEditLayout(tabWidget, "Points size"));
    QSpacerItem *tabSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    tab->addTab(tabWidget, "series " + QString::number(tab->count()));
    tabWidget->setLayout(tabLayout);
    tabLayout->addWidget(labelVisible);
    tabLayout->addWidget(labelPointsVisible);
    tabLayout->addWidget(labelPointsClipping);
    tabLayout->addLayout(labelPointsSize.constLast());
    tabLayout->addItem(tabSpacer);

    labelPointsClipping->setChecked(true);
    labelPointsSize.constLast()->setVisible(false);

    connect(labelVisible, &QCheckBox::toggled, this, &LabelSettingWidget::setPointsVisible);
    connect(labelVisible, &QCheckBox::toggled, labelPointsSize.constLast(), &LineEditLayout::setVisible);
    connect(labelPointsVisible, &QCheckBox::toggled, this, &LabelSettingWidget::setPointLabelsVisible);
    connect(labelPointsClipping, &QCheckBox::toggled, this, &LabelSettingWidget::setPointLabelsClipping);
    connect(labelPointsSize.constLast(), &LineEditLayout::lineTextEdited, this, &LabelSettingWidget::setPointsSize);
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
        rangeMaxEditConnection = connect(qobject_cast<QValueAxis*>(graph->axes(orient).constLast()), &QValueAxis::maxChanged, this, &AxisSettingGroupBox::setMaxEdit);
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








GraphicsTextItemSettingWidget::GraphicsTextItemSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    textEdit = new LineEditLayout(this, "Text");
    textSizeEdit = new SpinBoxEditLayout(this, "Size");
    rotationEdit = new LineEditLayout(this, "Rotation");
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(textEdit);
    layout->addLayout(textSizeEdit);
    layout->addLayout(rotationEdit);
    layout->addItem(spacer);

    connect(textEdit, &LineEditLayout::lineTextEdited, this, &GraphicsTextItemSettingWidget::setItemText);
    connect(textSizeEdit, &SpinBoxEditLayout::spinBoxValueChanged, this, &GraphicsTextItemSettingWidget::setItemTextSize);
}

void GraphicsTextItemSettingWidget::setGraphicsItem(GraphicsTextItem *const textItem)
{
    this->textItem = textItem;

    textEdit->setLineEditText(textItem->text());
    textSizeEdit->setSpinBoxValue(textItem->font().pointSize());
    rotationEdit->setLineEditText(QString::number(textItem->rotation()));
}

void GraphicsTextItemSettingWidget::setItemText(const QString& text)
{
    textItem->setText(text);
}

void GraphicsTextItemSettingWidget::setItemTextSize(const int ps)
{
    QFont textFont = textItem->font();
    textFont.setPointSize(ps);
    textItem->setFont(textFont);
}

void GraphicsTextItemSettingWidget::setItemRotation(const QString& rotation)
{
    textItem->setRotation(rotation.toDouble());
}





GraphicsLineItemSettingWidget::GraphicsLineItemSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *point1Layout = new QHBoxLayout;
    x1Edit = new LineEditLayout(this, "x1");
    y1Edit = new LineEditLayout(this, "y1");
    QHBoxLayout *point2Layout = new QHBoxLayout;
    x2Edit = new LineEditLayout(this, "x2");
    y2Edit = new LineEditLayout(this, "y2");
    lineWidthEdit = new SpinBoxEditLayout(this, "Width");
    styleEdit = new ComboEditLayout(this, "Style");
    customStyleEdit = new LineEditLayout(this, "Pattern");
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(point1Layout);
    point1Layout->addLayout(x1Edit);
    point1Layout->addLayout(y1Edit);
    layout->addLayout(point2Layout);
    point2Layout->addLayout(x2Edit);
    point2Layout->addLayout(y2Edit);
    layout->addLayout(lineWidthEdit);
    layout->addLayout(styleEdit);
    layout->addLayout(customStyleEdit);
    layout->addItem(spacer);

    x1Edit->setLabelMinimumWidth(0);
    y1Edit->setLabelMinimumWidth(0);
    x2Edit->setLabelMinimumWidth(0);
    y2Edit->setLabelMinimumWidth(0);
    styleEdit->insertComboItems(0, enumToStrings(ChartEnum::PenStyle(0)));
    customStyleEdit->setVisible(false);

    connect(x1Edit, &LineEditLayout::lineTextEdited, this, &GraphicsLineItemSettingWidget::setPoint1);
    connect(y1Edit, &LineEditLayout::lineTextEdited, this, &GraphicsLineItemSettingWidget::setPoint1);
    connect(x2Edit, &LineEditLayout::lineTextEdited, this, &GraphicsLineItemSettingWidget::setPoint2);
    connect(y2Edit, &LineEditLayout::lineTextEdited, this, &GraphicsLineItemSettingWidget::setPoint2);
    connect(lineWidthEdit, &SpinBoxEditLayout::spinBoxValueChanged, this, &GraphicsLineItemSettingWidget::setLineWidth);
    connect(styleEdit, &ComboEditLayout::currentComboIndexChanged, this, &GraphicsLineItemSettingWidget::setPenStyle);
    connect(customStyleEdit, &LineEditLayout::lineTextEdited, this, &GraphicsLineItemSettingWidget::setCustomDash);
}

void GraphicsLineItemSettingWidget::setGraphicsItem(GraphicsLineItem *const lineItem)
{
    this->lineItem = lineItem;

    x1Edit->setLineEditText(QString::number(lineItem->getChartP1Coord().x()));
    y1Edit->setLineEditText(QString::number(lineItem->getChartP1Coord().y()));
    x2Edit->setLineEditText(QString::number(lineItem->getChartP2Coord().x()));
    y2Edit->setLineEditText(QString::number(lineItem->getChartP2Coord().y()));
    lineWidthEdit->setSpinBoxValue(lineItem->pen().width());
}

void GraphicsLineItemSettingWidget::setPoint1()
{
    const QPointF p1ChartCoord = QPointF(x1Edit->lineEditText().toDouble(), y1Edit->lineEditText().toDouble());
    const QPointF p1ScenePos = lineItem->getChart()->mapToPosition(p1ChartCoord);
    const QPointF p1Coord = p1ScenePos - lineItem->pos();

    QLineF line = lineItem->line();
    line.setP1(p1Coord);
    lineItem->setLine(line);
}

void GraphicsLineItemSettingWidget::setPoint2()
{
    const QPointF p2ChartCoord = QPointF(x2Edit->lineEditText().toDouble(), y2Edit->lineEditText().toDouble());
    const QPointF p2ScenePos = lineItem->getChart()->mapToPosition(p2ChartCoord);
    const QPointF p2Coord = p2ScenePos - lineItem->pos();

    QLineF line = lineItem->line();
    line.setP2(p2Coord);
    lineItem->setLine(line);
}

void GraphicsLineItemSettingWidget::setLineWidth(const int lw)
{
    QPen pen = lineItem->pen();
    pen.setWidth(lw);
    lineItem->setPen(pen);
}

void GraphicsLineItemSettingWidget::setPenStyle(const int index)
{
    switch(ChartEnum::PenStyle(index))
    {
    case ChartEnum::PenStyle::Solid:
    case ChartEnum::PenStyle::Dash:
    case ChartEnum::PenStyle::Dot:
    case ChartEnum::PenStyle::DashDot:
    case ChartEnum::PenStyle::DashDotDot:{
        customStyleEdit->setVisible(false);
        break;
    }
    case ChartEnum::PenStyle::CustomDash:{
        customStyleEdit->setLineEditText("1");
        customStyleEdit->setVisible(true);
        break;
    }
    default:
        return; break;
    }

    QPen linePen = lineItem->pen();
    linePen.setStyle(Qt::PenStyle(index + 1));  //本来、Qt::PenStyleインデックス0にQt::NoPenがあるが、ChartEnum::PenStyleではないので、一つずらす
    lineItem->setPen(linePen);
}

void GraphicsLineItemSettingWidget::setCustomDash(const QString& _dashes)
{
    /* カンマ区切りで空白や語尾のカンマを処理して、QStringList型に変換する。 */
    QString dashes = _dashes;
    QStringList dashSeries = dashes.remove(' ').split(",");
    if(dashSeries.constLast().isEmpty()) dashSeries.removeLast();

    /* QStringList型をQList<qreal>型に変換 */
    QList<qreal> customDash;
    for(const QString& dash : dashSeries) customDash << dash.toDouble();

    /* dashPatternを設定 */
    QPen linePen = lineItem->pen();
    linePen.setDashPattern(customDash);
    lineItem->setPen(linePen);
}






















ItemSettingWidget::ItemSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *adderLayout = new QHBoxLayout;
    itemTypeCombo = new QComboBox(this);
    QPushButton *addItemButton = new QPushButton("Add", this);
    settingStack = new QStackedWidget(this);
    textItemWidget = new GraphicsTextItemSettingWidget(settingStack);
    lineItemWidget = new GraphicsLineItemSettingWidget(settingStack);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(adderLayout);
    adderLayout->addWidget(itemTypeCombo);
    adderLayout->addWidget(addItemButton);
    layout->addWidget(settingStack);
    settingStack->addWidget(textItemWidget);
    settingStack->addWidget(lineItemWidget);
    layout->addItem(spacer);

    addItemButton->setMaximumWidth(70);
    settingStack->setVisible(false);
}

template <class T>
void ItemSettingWidget::setItemSettingWidget(T *const item)
{
    settingStack->setVisible(true);
    const ChartEnum::ItemType type = item->itemType();
    settingStack->setCurrentIndex((int)type);

    switch (type)
    {
    case ChartEnum::ItemType::Text:
        textItemWidget->setGraphicsItem((GraphicsTextItem *const)item); break;
    case ChartEnum::ItemType::Line:
        lineItemWidget->setGraphicsItem((GraphicsLineItem *const)item); break;
    default:
        break;
    }
}









