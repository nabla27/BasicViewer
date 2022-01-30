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
    RGBEditLayout(QWidget *parent = nullptr);

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

#endif // GRAPH2DSERIES_H
