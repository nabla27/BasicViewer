#ifndef SERIALMONITOR_H
#define SERIALMONITOR_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextBrowser>
#include <QDebug>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QScreen>
#include <QComboBox>
#include "utility.h"
#include "serialmenubar.h"
#include "serialsettingwidget.h"
#include "serialplot1d.h"

class SerialMonitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit SerialMonitor(QWidget *parent = nullptr);

private slots:
    void initializeMenuBar();
    void initializeLayout();
    void setPort(const QSerialPortInfo& info);
    void openPort();
    void closePort();

private:
    QMetaObject::Connection readSerialConnection;
    void readSerial();

private:
    QSerialPort *serialPort;
    SerialSettingWidget *settingWidget;
    QTextBrowser *browser;
    PortOpeMenu *portOpeMenu;
    SerialPlot1D *graph1D;
    QString _buffer = "";

signals:

};

#endif // SERIALMONITOR_H
