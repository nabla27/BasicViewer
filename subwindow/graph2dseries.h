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

class Graph2DSeries : public QWidget
{
    Q_OBJECT
public:
    explicit Graph2DSeries(TableWidget *table, QWidget *parent = nullptr);
    ~Graph2DSeries();

private:
    void setTableSelectedIndex();
    void initializeGraph();
    void initializeGraphLayout();
    void setGraphSeries();
    void updateGraphData();

private:
    QChart *graph;
    QChartView *graphView;
    TableWidget *table;
    QString sheetName;
    QMetaObject::Connection changedTableAction;

    struct plotTableRange
    {
        plotTableRange(int startRow, int endRow, int colX, int colY)
            : startRow(startRow), endRow(endRow), colX(colX), colY(colY) {}
        int startRow = 0;
        int endRow = 0;
        int colX = 0 ;
        int colY = 0;
    };
    QList<plotTableRange> plotTableRanges;

    /* レイアウト */
    QVBoxLayout *legendBoxLayout;
    QList<QLineEdit*> legendNameEdit;
    static const QList<QString> colorNameList;
    static const QList<QString> themeNameList;
    static const QList<QString> imgFormatList();
    static const QColor rgbStrToColor(QString str);

private slots:
    void changeLegendVisible(bool visible);

signals:
    void graphSeriesUpdated();
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







class seriesSettingLayout : public QWidget
{
    Q_OBJECT

public:
    seriesSettingLayout(QWidget *parent, QChart *graph);

private:
    QChart *graph;
    QTabWidget *tab;
    QList<ComboEditLayout*> lineColorCombo;
    QList<RGBEditLayout*> lineColorCustom;

private slots:
    void setColorWithCombo(const int index);
    void setColorWithRGB(const QColor& color);
private:
    void setLineColor(const QColor& color);
};

class legendSettingLayout : public QWidget
{
    Q_OBJECT

public:
    legendSettingLayout(QWidget *parent, QChart *graph);

private:
    QChart *graph;

private slots:
    void setLegendPointSize(const int ps);
    void setLegendVisible(const bool visible) { graph->legend()->setVisible(visible); }
};

class LabelSettingLayout : public QWidget
{
    Q_OBJECT

public:
    LabelSettingLayout(QWidget *parent, QChart *graph);

private:
    QChart *graph;
    QTabWidget *tab;

private slots:
    void setPointsVisible(const bool visible);
    void setPointLabelsVisible(const bool visible);
    void setPointLabelsClipping(const bool visible);
};























#endif // GRAPH2DSERIES_H
