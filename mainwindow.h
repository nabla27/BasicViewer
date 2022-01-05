#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "gnuplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void loadXmlSetting();
    void saveXmlSetting();
    void reflectSetting();

private slots:
    void on_spinBox_setE_fs_valueChanged(int arg1);

    void on_fontComboBox_setE_f_textHighlighted(const QString &arg1);

    void on_checkBox_setE_ir_stateChanged(int arg1);

    void on_checkBox_setE_bl_stateChanged(int arg1);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_excute_released();

private:
    Ui::MainWindow *ui;
    Gnuplot *gnuplot;
};
#endif // MAINWINDOW_H
