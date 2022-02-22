#include "mainwindow.h"

#include <QApplication>

#include "window/gnuplot-editor/gnuploteditor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    GnuplotEditor *editor = new GnuplotEditor(nullptr);
    editor->show();

    return a.exec();
}
