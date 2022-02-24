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

    gnuplot = new ReGnuplot(this);

    connect(fileTree, &ReFileTree::scriptSelected, this, &GnuplotEditor::setEditorWidget);
    connect(fileTree, &ReFileTree::sheetSelected, this, &GnuplotEditor::setSheetWidget);
    connect(gnuplot, &ReGnuplot::standardOutputPassed, this, &GnuplotEditor::receiveGnuplotStdOut);
    connect(gnuplot, &ReGnuplot::standardErrorPassed, this, &GnuplotEditor::receiveGnuplotStdErr);
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar() != nullptr) delete menuBar();

    QMenuBar *menuBar = new QMenuBar(this);

    FileMenu *fileMenu = new FileMenu("File", menuBar);
    EditorMenu *editorMenu = new EditorMenu("Editor", menuBar);
    HelpMenu *helpMenu = new HelpMenu("Help", menuBar);
    QMenu *blank1 = new QMenu("         ", menuBar);
    scriptMenu = new ScriptMenu("Script", menuBar);
    sheetMenu = new SheetMenu("Sheet", menuBar);
    QMenu *blank2 = new QMenu("         ", menuBar);
    QAction *runAction = new QAction("&Run", menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editorMenu);
    menuBar->addMenu(helpMenu);
    menuBar->addMenu(blank1);
    menuBar->addMenu(scriptMenu);
    menuBar->addMenu(sheetMenu);
    menuBar->addMenu(blank2);
    menuBar->addAction(runAction);

    setMenuBar(menuBar);

    connect(runAction, &QAction::triggered, this, &GnuplotEditor::executeGnuplot);
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
    browserWidget = new BrowserWidget(centralWidget());
    /* 配置 */
    editorTab->addTab(gnuplotWidget, "&Gnuplot");
    editorTab->addTab(sheetWidget, "&Sheet");
    displayTab->addTab(consoleWidget, "&Console");
    displayTab->addTab(browserWidget, "&Output");
    /* 設定 */
    displayTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    displayTab->setMinimumHeight(150);
}

void GnuplotEditor::setEditorWidget(const QString& fileName, ReTextEdit *editor, QProcess *process)
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

    /* メニューバーの名前変更 */
    scriptMenu->setTitle(fileName);
}

void GnuplotEditor::setSheetWidget(const QString& fileName, ReTableWidget *sheet)
{
    /* 前にセットされてたものは削除 */
    if(sheetWidget->widget(sheetWidget->currentIndex()) != nullptr)
        sheetWidget->removeWidget(sheetWidget->currentWidget());

    /* 新しくセット */
    sheetWidget->addWidget(sheet);

    /* タブをSheetに設定 */
    editorTab->setCurrentIndex(1);

    /* メニューバーの名前変更 */
    sheetMenu->setTitle(fileName);
}

void GnuplotEditor::executeGnuplot()
{
    //ファイルが選ばれていない場合は無効
    if(gnuplotWidget->count() < 1) return;

    const QString script =  qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->toPlainText() + "\n";

    gnuplot->exc(gnuplotProcess, script.split("\n"));
}

void GnuplotEditor::receiveGnuplotStdOut(const QString& text)
{
    /* 出力の表示 */
    browserWidget->outputText(text, BrowserWidget::MessageType::GnuplotStdOut);

    /* タブの切り替え */
    displayTab->setCurrentIndex(1);
}

void GnuplotEditor::receiveGnuplotStdErr(const QString& text, const int line)
{
    /* 出力の表示 */
    browserWidget->outputText(text, BrowserWidget::MessageType::GnuplotStdErr);

    /* タブの切り替え */
    displayTab->setCurrentIndex(1);
}





























