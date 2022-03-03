#include "serialmonitor.h"

SerialMonitor::SerialMonitor(QWidget *parent)
    : QMainWindow{parent}
{
    serialPort = new QSerialPort(this);
    settingWidget = new SerialSettingWidget(nullptr); settingWidget->hide();

    initializeMenuBar();
    initializeLayout();

    setGeometry(getRectFromScreenRatio(screen()->size(), 0.3f, 0.4f));

    graph1D = new SerialPlot1D(nullptr);
}

void SerialMonitor::initializeMenuBar()
{
    if(menuBar()) delete menuBar();

    QMenuBar *menuBar = new QMenuBar(this);

    ToolMenu *toolMenu = new ToolMenu("Tool", menuBar);
    menuBar->addMenu(toolMenu);
    connect(toolMenu, &ToolMenu::portSelected, this, &SerialMonitor::setPort);

    SettingMenu *portSettingMenu = new SettingMenu("Port", menuBar);
    menuBar->addAction(portSettingMenu);
    connect(portSettingMenu, &QAction::triggered, settingWidget, &SerialSettingWidget::show);

    portOpeMenu = new PortOpeMenu("", menuBar);
    menuBar->addMenu(portOpeMenu);
    connect(portOpeMenu, &PortOpeMenu::portOpenSelected, this, &SerialMonitor::openPort);
    connect(portOpeMenu, &PortOpeMenu::portCloseSelected, this, &SerialMonitor::closePort);

    setMenuBar(menuBar);
}

void SerialMonitor::initializeLayout()
{
    if(centralWidget()) delete centralWidget();

    setCentralWidget(new QWidget(this));

    QVBoxLayout *layout = new QVBoxLayout(centralWidget());
    browser = new QTextBrowser(centralWidget());

    centralWidget()->setLayout(layout);
    layout->addWidget(browser);

    layout->setContentsMargins(0, 0, 0, 0);
}

void SerialMonitor::setPort(const QSerialPortInfo& info)
{
    serialPort->setPort(info);
    portOpeMenu->setTitle(info.portName());
}

void SerialMonitor::openPort()
{
    if(serialPort->open(QIODevice::ReadOnly))
    {
        readSerialConnection = connect(serialPort, &QSerialPort::readyRead, this, &SerialMonitor::readSerial);
        graph1D->show();
    }
    else
    {
        //開けなかった通知処理
    }
}

void SerialMonitor::closePort()
{
    serialPort->close();
    disconnect(readSerialConnection);
}

void SerialMonitor::readSerial()
{
    const QString data = serialPort->readAll();
    _buffer.append(data);

    browser->insertPlainText(data);

    if(_buffer.contains(','))
    {
        const QStringList sepaletedData = _buffer.split(',');
        for(const QString& value : sepaletedData){
            if(!value.isEmpty())
                graph1D->addData(value.toDouble());
                //qDebug() << value.toDouble();
        }
    }
}



































