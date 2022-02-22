#include "gnuploteditor.h"

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
{
    //メニュバーの生成
    initializeMenuBar();
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar() != nullptr) delete menuBar();

    QMenuBar *menuBar = new QMenuBar(nullptr);

    FileMenu *fileMenu = new FileMenu("File", menuBar);
    EditorMenu *editorMenu = new EditorMenu("Editor", menuBar);
    HelpMenu *helpMenu = new HelpMenu("Help", menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editorMenu);
    menuBar->addMenu(helpMenu);

    setMenuBar(menuBar);
}
