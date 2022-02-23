#include "gnuploteditor.h"

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
{
    //ウィンドウをスクリーン画面に対して(0.4,0.5)の比率サイズに設定
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.5f));

    //メニュバーの生成
    initializeMenuBar();

    //レイアウト生成
    initializeLayout();
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar() != nullptr) delete menuBar();

    QMenuBar *menuBar = new QMenuBar(this);

    FileMenu *fileMenu = new FileMenu("File", menuBar);
    EditorMenu *editorMenu = new EditorMenu("Editor", menuBar);
    HelpMenu *helpMenu = new HelpMenu("Help", menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editorMenu);
    menuBar->addMenu(helpMenu);

    setMenuBar(menuBar);
}

void GnuplotEditor::initializeLayout()
{
    if(centralWidget() != nullptr) delete centralWidget();

    setCentralWidget(new QWidget(this));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    fileTree = new ReFileTree(centralWidget());
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    editorTab = new QTabWidget(this);
    displayTab = new QTabWidget(centralWidget());

    centralWidget()->setLayout(hLayout);
    hLayout->addWidget(fileTree);
    hLayout->addLayout(vLayout);
    vLayout->addWidget(editorTab);
    vLayout->addWidget(displayTab);

    gnuplotWidget = new QWidget(centralWidget());
    sheetWidget = new QWidget(centralWidget());
    consoleWidget = new QWidget(centralWidget());
    stdoutWidget = new QWidget(centralWidget());

    editorTab->addTab(gnuplotWidget, "Gnuplot");
    editorTab->addTab(sheetWidget, "Sheet");
    displayTab->addTab(consoleWidget, "Console");
    displayTab->addTab(stdoutWidget, "StdOut");
}
