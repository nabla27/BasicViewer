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

    FileMenu *const fileMenu = new FileMenu("File", menuBar);
    EditorMenu *const editorMenu = new EditorMenu("Editor", menuBar);
    HelpMenu *const helpMenu = new HelpMenu("Help", menuBar);
    QMenu *const blank1 = new QMenu("         ", menuBar);
    scriptMenu = new ScriptMenu("Script", menuBar);
    sheetMenu = new SheetMenu("Sheet", menuBar);
    QMenu *const blank2 = new QMenu("         ", menuBar);
    QAction *const runAction = new QAction("&Run", menuBar);

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
    QHBoxLayout *hLayout = new QHBoxLayout;
    fileTree = new ReFileTree(centralWidget());
    QVBoxLayout *vLayout = new QVBoxLayout;
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
    gnuplotWidget = new QStackedWidget(editorTab);
    sheetWidget = new QStackedWidget(editorTab);
    consoleWidget = new QWidget(displayTab);
    browserWidget = new BrowserWidget(displayTab);
    /* 配置 */
    editorTab->addTab(gnuplotWidget, "&Gnuplot");
    editorTab->addTab(sheetWidget, "&Sheet");
    displayTab->addTab(consoleWidget, "&Console");
    displayTab->addTab(browserWidget, "&Output");
    /* 設定 */
    displayTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    displayTab->setMaximumHeight(150);

    /* 配色設定 */
    setPalette(QPalette(QPalette::Window, Qt::black));
    fileTree->setPalette(QPalette(QPalette::Window, Qt::white));
    for(int i = 0; i < editorTab->count(); ++i) editorTab->tabBar()->setTabTextColor(i, Qt::black);
    for(int i = 0; i < displayTab->count(); ++i) displayTab->tabBar()->setTabTextColor(i, Qt::black);
    sheetWidget->setPalette(QPalette(QPalette::Window, Qt::white));
    browserWidget->setPalette(QPalette(QPalette::Window, Qt::white));
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

void GnuplotEditor::setFolderPath(const QString& folderPath)
{
    fileTree->setFolderPath(folderPath);
    fileTree->loadFileTree();

    gnuplot->setWorkingPath(folderPath);
}

void GnuplotEditor::executeGnuplot()
{
    //ファイルが選ばれていない場合は無効
    if(gnuplotWidget->count() < 1) return;

    /* エラー行のリセット */
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->setErrorLineNumber(-1);

    /* ファイルの保存 */
    fileTree->scriptList.saveScript(fileTree->getFolderPath(), scriptMenu->title());
    fileTree->scriptList.saveScript(fileTree->getFolderPath(), sheetMenu->title());

    /* gnuplotにコマンドを渡す */
    gnuplot->exc(gnuplotProcess, QList<QString>() << "load '" + scriptMenu->title() + "'");
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

    /* エラー行の設定とハイライト */
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->setErrorLineNumber(line - 1);
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->highlightLine();
}





























