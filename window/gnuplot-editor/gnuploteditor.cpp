#include "gnuploteditor.h"

GnuplotEditor::GnuplotEditor(QWidget *parent)
    : QMainWindow(parent)
{
    //ウィンドウをスクリーン画面に対して(0.4,0.5)の比率サイズに設定
    setGeometry(getRectFromScreenRatio(screen()->size(), 0.4f, 0.5f));

    //レイアウト生成
    initializeLayout();

    //メニュバーの生成
    initializeMenuBar();

    gnuplot = new ReGnuplot(this);
    editorSetting = new EditorSettingWidget(this);

    connect(fileTree, &ReFileTree::scriptSelected, this, &GnuplotEditor::setEditorWidget);
    connect(fileTree, &ReFileTree::sheetSelected, this, &GnuplotEditor::setSheetWidget);
    connect(fileTree, &ReFileTree::otherSelected, this, &GnuplotEditor::setOtherWidget);
    connect(fileTree, &ReFileTree::scriptRemoved, this, &GnuplotEditor::setEditorWidget);
    connect(fileTree, &ReFileTree::sheetRemoved, this, &GnuplotEditor::setSheetWidget);
    connect(fileTree, &ReFileTree::otherRemoved, this, &GnuplotEditor::setOtherWidget);
    connect(fileTree, &ReFileTree::fileNameChanged, this, &GnuplotEditor::setMenuBarTitle);
    connect(fileTree, &ReFileTree::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(gnuplot, &ReGnuplot::standardOutputPassed, this, &GnuplotEditor::receiveGnuplotStdOut);
    connect(gnuplot, &ReGnuplot::standardErrorPassed, this, &GnuplotEditor::receiveGnuplotStdErr);
    connect(gnuplot, &ReGnuplot::errorCaused, browserWidget, &BrowserWidget::outputText);
    connect(browserWidget, &BrowserWidget::textChanged, [this](){ displayTab->setCurrentIndex(1); });
}

void GnuplotEditor::initializeMenuBar()
{
    if(menuBar()) delete menuBar();

    WindowMenuBar *menuBar = new WindowMenuBar(this);

    FileMenu *const fileMenu = new FileMenu("File", menuBar);
    WidgetMenu *const widgetMenu = new WidgetMenu("Widget", menuBar);
    HelpMenu *const helpMenu = new HelpMenu("Help", menuBar);
    QAction *const blank1 = new QAction("         ", menuBar);
    scriptMenu = new ScriptMenu("      ", menuBar);
    sheetMenu = new SheetMenu("     ", menuBar);
    QAction *const blank2 = new QAction("         ", menuBar);
    QAction *const runAction = new QAction("&Run", menuBar);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(widgetMenu);
    menuBar->addMenu(helpMenu);
    menuBar->addAction(blank1);
    menuBar->addMenu(scriptMenu);
    menuBar->addMenu(sheetMenu);
    menuBar->addAction(blank2);
    menuBar->addAction(runAction);

    setMenuBar(menuBar);

    connect(fileMenu, &FileMenu::reloadFolderPushed, fileTree, &ReFileTree::loadFileTree);
    connect(fileMenu, &FileMenu::updateFolderPushed, fileTree, &ReFileTree::updateFileTree);
    connect(fileMenu, &FileMenu::openFolderPushed, fileTree, &ReFileTree::setFolderPath);
    connect(fileMenu, &FileMenu::addFolderPushed, fileTree, &ReFileTree::addFolder);
    connect(fileMenu, &FileMenu::saveFolderPushed, fileTree, &ReFileTree::saveFolder);
    connect(widgetMenu, &WidgetMenu::clearOutputWindowPushed, browserWidget, &BrowserWidget::clear);
    connect(widgetMenu, &WidgetMenu::editorSettingOpened, editorSetting, &EditorSettingWidget::show);
    //connect(widgetMenu, &WidgetMenu::clearConsoleWindowPushed, consoleWidget, &);
    connect(runAction, &QAction::triggered, this, &GnuplotEditor::executeGnuplot);
}

void GnuplotEditor::initializeLayout()
{
    if(centralWidget()) delete centralWidget();

    setCentralWidget(new QWidget(this));

    /* ウィンドウ内の大枠となるレイアウトとウィジェットの初期化 */
    QHBoxLayout *hLayout = new QHBoxLayout;
    fileTree = new ReFileTree(centralWidget());
    VerticalDragBar *fileTreeDragBar = new VerticalDragBar(centralWidget());
    QVBoxLayout *vLayout = new QVBoxLayout;
    editorTab = new QTabWidget(this);
    HorizontalDragBar *displayTabDragBar = new HorizontalDragBar(centralWidget());
    displayTab = new QTabWidget(centralWidget());
    /* 配置 */
    centralWidget()->setLayout(hLayout);
    hLayout->addWidget(fileTree);
    hLayout->addWidget(fileTreeDragBar);
    hLayout->addLayout(vLayout);
    vLayout->addWidget(editorTab);
    vLayout->addWidget(displayTabDragBar);
    vLayout->addWidget(displayTab);
    /* 設定 */
    hLayout->setSpacing(0);
    fileTree->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    fileTree->setMaximumWidth(150);
    vLayout->setSpacing(0);

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
    editorTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    displayTab->setTabPosition(QTabWidget::TabPosition::South);
    browserWidget->setFrameShape(QFrame::Shape::NoFrame);

    /* 配色設定 */
    //setPalette(QPalette(QPalette::Window, Qt::black));
    //fileTree->setPalette(QPalette(QPalette::Window, Qt::white));
    //for(int i = 0; i < editorTab->count(); ++i) editorTab->tabBar()->setTabTextColor(i, Qt::black);
    //for(int i = 0; i < displayTab->count(); ++i) displayTab->tabBar()->setTabTextColor(i, Qt::black);
    //sheetWidget->setPalette(QPalette(QPalette::Window, Qt::white));
    //browserWidget->setPalette(QPalette(QPalette::Window, Qt::white));

    connect(fileTreeDragBar, &VerticalDragBar::barDraged, this, &GnuplotEditor::setFileTreeWidth);
    connect(displayTabDragBar, &HorizontalDragBar::barDraged, this, &GnuplotEditor::setDisplayTabHeight);
}

void GnuplotEditor::setEditorWidget(const QString& fileName, const ScriptInfo* info)
{
    if(!info){  //infoがnullptr(ツリーから消された場合など)なら
        if(scriptMenu->title() == fileName) scriptMenu->setTitle("      ");
        return;
    }

    /* 前にセットされてたものは削除 */
    if(gnuplotWidget->widget(gnuplotWidget->currentIndex()))
        gnuplotWidget->removeWidget(gnuplotWidget->currentWidget());

    /* 新しくセット */
    gnuplotWidget->addWidget(info->editor);       //editorのparentは自動的にgnuplotWidgetとなる

    /* プロセスをセット */
    gnuplotProcess = info->process;

    /* タブをGnuplotに設定 */
    editorTab->setCurrentIndex(0);

    /* メニューバーの名前変更 */
    scriptMenu->setTitle(fileName);
}

void GnuplotEditor::setSheetWidget(const QString& fileName, const SheetInfo* info)
{
    if(!info){ //infoがnullptr(ツリーから消された場合など)
        if(sheetMenu->title() == fileName) sheetMenu->setTitle("     ");
        return;
    }

    /* 前にセットされてたものは削除 */
    if(sheetWidget->widget(sheetWidget->currentIndex()))
        sheetWidget->removeWidget(sheetWidget->currentWidget());

    /* 新しくセット */
    sheetWidget->addWidget(info->table);
    info->table->setGnuplot(gnuplot);

    /* タブをSheetに設定 */
    editorTab->setCurrentIndex(1);

    /* メニューバーの名前変更 */
    sheetMenu->setTitle(fileName);
}

void GnuplotEditor::setOtherWidget(const QString& fileName, const OtherInfo* info)
{

}

void GnuplotEditor::setMenuBarTitle(const QString& oldName, const QString& newName)
{
    if(scriptMenu->title() == oldName) scriptMenu->setTitle(newName);
    else if(sheetMenu->title() == oldName) sheetMenu->setTitle(newName);
}

void GnuplotEditor::setFolderPath(const QString& folderPath)
{
    fileTree->setFolderPath(folderPath);

    gnuplot->setWorkingPath(folderPath);
}

void GnuplotEditor::executeGnuplot()
{
    /* browserの過去の出力をグレイアウト */
    browserWidget->grayOutAll();

    /* ファイルが選ばれていない場合は無効 */
    if(gnuplotWidget->count() < 1) {
        browserWidget->outputText("scripts are not selected.", BrowserWidget::MessageType::SystemErr);
        return;
    }

    /* エラー行のリセット */
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->setErrorLineNumber(-1);
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->highlightLine();

    /* ファイルの保存 */
    fileTree->saveScript(scriptMenu->title());
    fileTree->saveSheet(sheetMenu->title());

    /* gnuplotにコマンドを渡す */
    gnuplot->exc(gnuplotProcess, QList<QString>() << "load '" + scriptMenu->title() + "'");
}

void GnuplotEditor::receiveGnuplotStdOut(const QString& text)
{
    /* 出力の表示 */
    browserWidget->outputText(text, BrowserWidget::MessageType::GnuplotStdOut);
}

void GnuplotEditor::receiveGnuplotStdErr(const QString& text, const int line)
{
    /* 出力の表示 */
    browserWidget->outputText(text, BrowserWidget::MessageType::GnuplotStdErr);

    /* エラー行の設定とハイライト */
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->setErrorLineNumber(line - 1);
    qobject_cast<ReTextEdit*>(gnuplotWidget->widget(0))->highlightLine();
}

void GnuplotEditor::setFileTreeWidth(const int dx)
{
    const int nextWidth = fileTree->maximumWidth() - dx;
    if(nextWidth < 0 || nextWidth > 260) return;
    fileTree->setMaximumWidth(nextWidth);
}

void GnuplotEditor::setDisplayTabHeight(const int dy)
{
    const int nextHeight = displayTab->maximumHeight() + dy;
    if(nextHeight < 0 || nextHeight > 600) return;
    displayTab->setMaximumHeight(nextHeight);
}






























