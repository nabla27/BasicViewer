#include "gnuploteditor.h"

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
{
    QMenuBar *menuBar = new QMenuBar(nullptr);

    FileMenu *fileMenu = new FileMenu("File", menuBar);
    menuBar->addMenu(fileMenu);

    setMenuBar(menuBar);

}
