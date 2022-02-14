#ifndef GRAPH2DSERIES_H
#define GRAPH2DSERIES_H
#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineSeries>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QToolBox>
#include <QLabel>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QComboBox>
#include <QSpacerItem>
#include <QValueAxis>
#include <QSpinBox>
#include <QPushButton>
#include <QPixmap>
#include <QImageWriter>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QColor>
#include "tablewidget.h"

#include <QSplineSeries>
#include <QBoxPlotSeries>
#include <QAreaSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QCandlestickSeries>
#include <QCandlestickSet>
#include <QPieSeries>
#include <QPieSlice>
#include <QLogValueAxis>
#include <QScatterSeries>
#include <QPointF>
#include <QMetaEnum>

template <typename QEnum>
QString enumToString(const QEnum value) { return QString(QMetaEnum::fromType<QEnum>().valueToKey((int)value)); }

template <class QEnum>
QStringList enumToStrings(const QEnum){
    QStringList enumStrList;
    const int enumCount = QMetaEnum::fromType<QEnum>().keyCount();
    for(int i = 0; i < enumCount; ++i){
        enumStrList << QStringList(QMetaEnum::fromType<QEnum>().valueToKey(i));
    }

    return enumStrList;
}

template <class QEnum>
int getEnumCount(const QEnum){
    return QMetaEnum::fromType<QEnum>().keyCount();
}

template <class QEnum>
int getEnumIndex(const QString enumStr){
    const int enumCount = getEnumCount(QEnum(0));
    for(int i = 0; i < enumCount; ++i){
        if(QMetaEnum::fromType<QEnum>().valueToKey(i) == enumStr) { return i; }
    }

    return -1;
}

class CEnum : private QObject{
    Q_OBJECT
public:
    enum class PlotType { LineSeries, SplineSeries, ScatterSeries, AreaSeries, LogressionLine };
    enum class MarkerShape { Circle, Rectangle, RotatedRectangle, Triangle, ShapeStar };
    Q_ENUM(PlotType)
    Q_ENUM(MarkerShape)
};



struct SeriesData
{
    CEnum::PlotType plotType = CEnum::PlotType::LineSeries;
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


class Graph2DSeries : public QWidget
{
    Q_OBJECT
public:
    explicit Graph2DSeries(TableWidget *table, QWidget *parent = nullptr);
    ~Graph2DSeries();

private:
    QChart *graph;
    QChartView *graphView;
    TableWidget *table;
    QString sheetName;
    QMetaObject::Connection changedTableAction;
    QList<SeriesData> seriesData;
    QList<PlotTableRange> plotTableRanges;

protected :
    void wheelEvent(QWheelEvent *event);

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
    void changeSeriesType(const CEnum::PlotType type, const int index = -1);
    void addNewSeries(const int index, const CEnum::PlotType type);
    void addSeriesToGraph(const SeriesData& data);
};







class RGBEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    RGBEditLayout(QWidget *parent);

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setEditMaximumWidth(const int width) { rEdit->setMaximumWidth(width);
                                                gEdit->setMaximumWidth(width);
                                                bEdit->setMaximumWidth(width); }

public slots:
    void setColor(const QColor& color);
    void setColor(int eNum);
    void setVisible(bool visible);
    void setReadOnly(bool readOnly);

signals:
    void colorEdited(const QColor& color);

private:
    QColor getColor() { return QColor(rEdit->text().toInt(), gEdit->text().toInt(), bEdit->text().toInt()); }

private:
    QLabel *label;
    QLineEdit *rEdit;
    QLineEdit *gEdit;
    QLineEdit *bEdit;
    QSpacerItem *spacer;
};

class ComboEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    ComboEditLayout(QWidget *parent, const QString& text = "");

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setComboMaximumWidth(const int width) { combo->setMaximumWidth(width); }
    void insertComboItems(int index, const QStringList& texts) { combo->insertItems(index, texts); }
    int currentComboIndex() const { return combo->currentIndex(); }
    QString currentComboText() const { return combo->currentText(); }

public slots:
    void setComboCurrentIndex(const int index) { combo->setCurrentIndex(index); }
    void setVisible(bool visible);

signals:
    void currentComboIndexChanged(int index);

private:
    QLabel *label;
    QComboBox *combo;
    QSpacerItem *spacer;
};

class LineEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    LineEditLayout(QWidget *parent, const QString& text = "");

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setLineEditMaximumWidth(const int width) { lineEdit->setMaximumWidth(width); }
    QString lineEditText() const { return lineEdit->text(); }

public slots:
    void setLineEditText(const QString& text) { lineEdit->setText(text); }
    void setVisible(bool visible);

signals:
    void lineTextEdited(const QString& text);

private:
    QLabel *label;
    QLineEdit *lineEdit;
    QSpacerItem *spacer;
};

class SpinBoxEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    SpinBoxEditLayout(QWidget *parent, const QString& text = "");

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setSpinBoxMaximumWidth(const int width) { spinBox->setMaximumWidth(width); }
    int spinBoxValue() const { return spinBox->value(); }

public slots:
    void setSpinBoxValue(const int value) { spinBox->setValue(value); }
    void setVisible(bool visible);

signals:
    void spinBoxValueChanged(const int value);

private:
    QLabel *label;
    QSpinBox *spinBox;
    QSpacerItem *spacer;
};

class PushButtonLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    PushButtonLayout(QWidget *parent, const QString& text = ""){
        button = new QPushButton(text, parent);
        addWidget(button, 0, Qt::AlignCenter);
        connect(button, &QPushButton::released, [this](){ emit buttonReleased(); });
    };

public:
    void setButtonMaximumWidth(const int width) { button->setMaximumWidth(width); }
    void setButtonMinimumWidth(const int width) { button->setMinimumWidth(width); }

private:
    QPushButton *button;

signals:
    void buttonReleased();
};

class BlankSpaceLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    BlankSpaceLayout(const int width, const int height){
        spacer = new QSpacerItem(width, height, QSizePolicy::Maximum, QSizePolicy::Maximum);
        addItem(spacer);
    }

private:
    QSpacerItem *spacer;
};







class GraphSettingWidget : public QWidget
{
    Q_OBJECT

public:
    GraphSettingWidget(QWidget *parent, QChart *graph);

private:
    QChart *graph;

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
    void addTab(CEnum::PlotType type);
    void changeWidgetItemVisible(const CEnum::PlotType type, const int index);
private:
    void setLineColor(const QColor& color);
    const QColor getLineColor(const int index) const;
signals:
    void seriesTypeChanged(const CEnum::PlotType type, const int index);
    void newSeriesAdded(const int index, const CEnum::PlotType type);
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




















#endif // GRAPH2DSERIES_H
