#ifndef GRAPH2DSERIES_H
#define GRAPH2DSERIES_H
#include <QChart>
#include <QChartView>

#include <QLineSeries>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QAreaSeries>

#include <QValueAxis>
#include <QLogValueAxis>

#include <QGraphicsSceneHelpEvent>

#include <QPixmap>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPointF>

#include "tablewidget.h"
#include "layoutparts.h"
#include "utility.h"



class ChartEnum : private QObject{
    Q_OBJECT
public:
    enum class PlotType { LineSeries, SplineSeries, ScatterSeries, AreaSeries, LogressionLine };
    enum class MarkerShape { Circle, Rectangle, RotatedRectangle, Triangle, ShapeStar };
    enum class Theme { Light, BlueCerulean, Dark, BrownSand, BlueNcs, HighContrast, BlueIcy, Qt };
    enum class ItemType { Text };
    Q_ENUM(PlotType)
    Q_ENUM(MarkerShape)
    Q_ENUM(Theme)
    Q_ENUM(ItemType)
};



struct SeriesData
{
    ChartEnum::PlotType plotType = ChartEnum::PlotType::LineSeries;
    qsizetype rangeIndex = 0;
};

struct PlotTableRange
{
    PlotTableRange(int startRow, int endRow, int colX, int colY)
        : startRow(startRow), endRow(endRow), colX(colX), colY(colY) {}
    int startRow = 0;
    int endRow = 0;
    int colX = 0 ;
    int colY = 0;
    bool isInRange(const int row, const int col) const{
        if(colX <= col && colY >= col && startRow <= row && endRow >= row) { return true; }
        return false;
    }
};

class ChartView : public QChartView
{
    Q_OBJECT

public:
    ChartView(QChart *chart, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void moveGraph(const QPoint& cursorPos);

    struct DragStartPoint{
        QPointF min;
        QPointF max;
        QPointF pixelWidth;
        QPoint pos;
    } dragStartPoint;

    QMenu *itemMenu;

private slots:
    void addTextItem();

signals:
    void mouseCoordinateMoved(const QString& x, const QString& y);
};


class ItemSettingWidget;

class GraphicsTextItem : public QGraphicsSimpleTextItem
{
public:
    GraphicsTextItem(const QString& text, QGraphicsItem *parent = nullptr);

public:
    static const ChartEnum::ItemType itemType() { return ChartEnum::ItemType::Text; }
    static void setSettingWidget(ItemSettingWidget *widget);

protected:
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    static ItemSettingWidget *settingWidget;
};




class Graph2DSeries : public QWidget
{
    Q_OBJECT
public:
    explicit Graph2DSeries(TableWidget *table, QWidget *parent = nullptr);
    ~Graph2DSeries();

private:
    QChart *graph;
    ChartView *graphView;
    TableWidget *table;
    QString sheetName;
    QMetaObject::Connection changedTableAction;
    QList<SeriesData> seriesData;
    QList<PlotTableRange> plotTableRanges;

private:
    void setTableSelectedIndex();                    //tableの選択範囲を設定
    void initializeGraphSeries();                    //グラフのseriesの初期化
    void initializeGraphLayout();                    //グラフのレイアウトの初期化
    void updateGraphSeries(QTableWidgetItem *item);  //table変更に伴うグラフの再描画

    //指定したQXYSeriesを継承したT(QLineSeries,QSplineSeries,QScatterSeries)型のSeriesを返す
    template<class T> T* createXYSeries(const PlotTableRange& range);
    //直線以下を塗りつぶすQAreaSeries型のseriesを返す
    QAreaSeries* createAreaSeries(const SeriesData& data);
    //最小二乗法による近似曲線を求める
    QPointF deriveRogressionLine(const PlotTableRange& range);
    QLineSeries* createRogressionLine(const PlotTableRange& range);

    qreal getRangeMin(Qt::Orientation orient);
    qreal getRangeMax(Qt::Orientation orient);

    void updateRogressionLine(const int index);

private slots:
    void changeSeriesType(const ChartEnum::PlotType type, const int index = -1);
    void addNewSeries(const int index, const ChartEnum::PlotType type);
    void addSeriesToGraph(const SeriesData& data);
};












class GraphSettingWidget : public QWidget
{
    Q_OBJECT

public:
    GraphSettingWidget(QWidget *parent, QChart *graph);

public slots:
    void setCoordinateValue(const QString& x, const QString& y);

private:
    QChart *graph;
    LineEditLayout *xCoordinate;
    LineEditLayout *yCoordinate;

private slots:
    void setTitleSize(const int ps);
    void setTheme(const int index) { graph->setTheme(QChart::ChartTheme(index)); }
};

class SeriesSettingWidget : public QWidget
{
    Q_OBJECT

public:
    SeriesSettingWidget(QWidget *parent, QChart *graph);

private:
    QChart *graph;
    QTabWidget *tab;
    QList<ComboEditLayout*> seriesTypeCombo;
    QList<ComboEditLayout*> lineColorCombo;
    QList<RGBEditLayout*> lineColorCustom;
    QList<ComboEditLayout*> scatterTypeCombo;

private slots:
    void setLineVisible(const bool visible);
    void setColorWithCombo(const int index);
    void setColorWithRGB(const QColor& color);
    void emitSeriesTypeChanged(const int type);
    void setScatterType(const int type);
    void addNewSeries();
    void addTab(ChartEnum::PlotType type);
    void changeWidgetItemVisible(const ChartEnum::PlotType type, const int index);
private:
    void setLineColor(const QColor& color);
    const QColor getLineColor(const int index) const;
signals:
    void seriesTypeChanged(const ChartEnum::PlotType type, const int index);
    void newSeriesAdded(const int index, const ChartEnum::PlotType type);
};

class LegendSettingWidget : public QWidget
{
    Q_OBJECT

public:
    LegendSettingWidget(QWidget *parent, QChart *graph);

private:
    QChart *graph;
    QVBoxLayout *layout;
    QCheckBox *legendVisible;
    QList<LineEditLayout*> legendName;

public slots:
    void reconnectSeriesNameEditor();
    void addSeriesNameEditer();

private slots:
    void setLegendPointSize(const int ps);
    void setLegendVisible(const bool visible) { graph->legend()->setVisible(visible); }
};

class LabelSettingWidget : public QWidget
{
    Q_OBJECT

public:
    LabelSettingWidget(QWidget *parent, QChart *graph);

private:
    QChart *graph;
    QTabWidget *tab;
    QList<LineEditLayout*> labelPointsSize;

public slots:
    void addTab();

private slots:
    void setPointsVisible(const bool visible);
    void setPointLabelsVisible(const bool visible);
    void setPointLabelsClipping(const bool visible);
    void setPointsSize(const QString& ps);
};

class AxisSettingGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    AxisSettingGroupBox(QWidget *parent, QChart *graph, const QString& text, const Qt::Orientation orient);

private:
    QChart *graph;
    Qt::Orientation orient;
    LineEditLayout *rangeMin;
    LineEditLayout *rangeMax;
    SpinBoxEditLayout *axisNameSize;
    LineEditLayout *labelAngle;
    ComboEditLayout *labelColor;
    RGBEditLayout *labelColorCustom;
    SpinBoxEditLayout *labelSize;
    ComboEditLayout *gridColor;
    RGBEditLayout *gridColorCustom;

    QMetaObject::Connection rangeMaxEditConnection;
    QMetaObject::Connection rangeMinEditConnection;

public slots:
    void connectRangeEdit();
    void setRangeEdit();

private slots:
    void setMinEdit(const qreal min) { rangeMin->setLineEditText(QString::number(min)); }
    void setMaxEdit(const qreal max) { rangeMax->setLineEditText(QString::number(max)); }
    void setAxisNameSize(const int ps);
    void setAxisLabelAngle(const QString& angle);
    void setAxisLabelColor(const int index);
    void setAxisLabelSize(const int ps);
    void setGridColor(const int index);
};

class AxisSettingWidget : public QWidget
{
    Q_OBJECT

public:
    AxisSettingWidget(QWidget *parent, QChart *graph);

public:
    AxisSettingGroupBox *xAxisGroupBox;
    AxisSettingGroupBox *yAxisGroupBox;

private:
    QChart *graph;
};

class ExportSettingWidget : public QWidget
{
    Q_OBJECT

public:
    ExportSettingWidget(QWidget *parent, QChart *graph, QChartView *graphView);

private:
    QChart *graph;
    QChartView *graphView;
    ComboEditLayout *type;
    LineEditLayout *fileName;
    ComboEditLayout *imageFormat;

private slots:
    void exportFile();
    void exportToImage();
};



class GraphicsTextItemSettingWidget : public QWidget
{
    Q_OBJECT

public:
    GraphicsTextItemSettingWidget(QWidget *parent = nullptr);
    void setGraphicsItem(GraphicsTextItem *const textItem);

private slots:
    void setItemText(const QString& text);
    void setItemTextSize(const int ps);
    void setItemRotation(const QString& rotation);

private:
    GraphicsTextItem *textItem;
    LineEditLayout *textEdit;
    SpinBoxEditLayout *textSizeEdit;
    LineEditLayout *rotationEdit;
};



class ItemSettingWidget : public QWidget
{
    Q_OBJECT

public:
    ItemSettingWidget(QWidget *parent = nullptr);

public:
    template <class T> void setItemSettingWidget(T *const item);

private:
    QGraphicsItem *item;
    QComboBox *itemTypeCombo;
    QStackedWidget *settingStack;
    GraphicsTextItemSettingWidget *textItemWidget;
};













#endif // GRAPH2DSERIES_H
