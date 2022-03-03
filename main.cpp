#include "mainwindow.h"

#include <QApplication>

#include "window/gnuplot-editor/gnuploteditor.h"
#include "window/serial-monitor/serialmonitor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
#if 0
    GnuplotEditor *editor = new GnuplotEditor(nullptr);
    editor->show();
#else
    SerialMonitor *monitor = new SerialMonitor(nullptr);
    monitor->show();
#endif
    return a.exec();
}
