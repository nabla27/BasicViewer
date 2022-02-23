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

    connect(fileTree, &ReFileTree::scriptSelected, this, &GnuplotEditor::setEditorWidget);
    connect(fileTree, &ReFileTree::sheetSelected, this, &GnuplotEditor::setSheetWidget);
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

    /* ウィンドウ内の大枠となるレイアウトとウィジェットの初期化 */
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    fileTree = new ReFileTree(centralWidget());
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    editorTab = new QTabWidget(this);
    displayTab = new QTabWidget(centralWidget());
    /* 配置 */
    centralWidget()->setLayout(hLayout);
    hLayout->addWidget(fileTree);
    hLayout->addLayout(vLayout);
    vLayout->addWidget(editorTab);
    vLayout->addWidget(displayTab);
    /* 設定 */
    fileTree->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    fileTree->setMaximumWidth(150);

    /* 各ウィジェット内のアイテムの初期化 */
    gnuplotWidget = new QStackedWidget(centralWidget());
    sheetWidget = new QStackedWidget(centralWidget());
    consoleWidget = new QWidget(centralWidget());
    outputWidget = new QWidget(centralWidget());
    /* 配置 */
    editorTab->addTab(gnuplotWidget, "Gnuplot");
    editorTab->addTab(sheetWidget, "Sheet");
    displayTab->addTab(consoleWidget, "Console");
    displayTab->addTab(outputWidget, "Output");
    /* 設定 */
    displayTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    displayTab->setMinimumHeight(150);
}

void GnuplotEditor::setEditorWidget(ReTextEdit *editor, QProcess *process)
{
    /* 前にセットされてたものは削除 */
    if(gnuplotWidget->widget(gnuplotWidget->currentIndex()) != nullptr)
        gnuplotWidget->removeWidget(gnuplotWidget->currentWidget());

    /* 新しくセット */
    gnuplotWidget->addWidget(editor);       //editorのparentは自動的にgnuplotWidgetとなる

    /* プロセスをセット */
    gnuplotProcess = process;

    /* タブをGnuplotに設定 */
    editorTab->setCurrentIndex(0);
}

void GnuplotEditor::setSheetWidget(ReTableWidget *sheet)
{
    /* 前にセットされてたものは削除 */
    if(sheetWidget->widget(sheetWidget->currentIndex()) != nullptr)
        sheetWidget->removeWidget(sheetWidget->currentWidget());

    /* 新しくセット */
    sheetWidget->addWidget(sheet);

    /* タブをSheetに設定 */
    editorTab->setCurrentIndex(1); qDebug() << __LINE__;
}





























