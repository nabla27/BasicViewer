#include "refiletree.h"




QString ScriptInfo::format = ".txt";
QString SheetInfo::format = ".csv";

ReFileTree::ReFileTree(QWidget *parent)
    : QTreeWidget(parent)/*, scriptList(parent), sheetList(parent), otherList(parent)*/
{
    /* 右クリックメニューの設定 */
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    normalMenu = new QMenu(this);
    connect(this, &ReFileTree::customContextMenuRequested, this, &ReFileTree::onCustomContextMenu);

    /* ファイルの変更があれば更新するようにする */
    dirWatcher = new QFileSystemWatcher(QStringList() << folderPath);
    connect(dirWatcher, &QFileSystemWatcher::directoryChanged, this, &ReFileTree::updateFileTree);

    /* ファイルツリーを更新 */
    loadFileTree();

    connect(this, &ReFileTree::itemDoubleClicked, this, &ReFileTree::pushClickedItem);
}

void ReFileTree::loadFileTree()
{
    /* treeをクリア */
    clear();

    /* ヘッダーを非表示 */
    setHeaderHidden(true);

    /* scriptフォルダーを作成 */
    scriptTree = new QTreeWidgetItem(this);
    scriptTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    scriptTree->setText(0, "Script");
    /* sheetフォルダーの作成 */
    sheetTree = new QTreeWidgetItem(this);
    sheetTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    sheetTree->setText(0, "Sheet");
    /* otherフォルダーの作成 */
    otherTree = new QTreeWidgetItem(this);
    otherTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    otherTree->setText(0, "Other");

    /* ファイルの追加 */
    updateFileTree();
}

void ReFileTree::updateFileTree()
{
    /* ディレクトリのファイル情報を取得 */
    QDir dir(folderPath);
    QFileInfoList fileList = dir.entryInfoList();

    /* ファイルを順に捜査して、拡張子ごとにフォルダーに分ける。 */
    for(const QFileInfo& fileInfo : fileList)
    {
        const QString fileName = fileInfo.fileName();

        if(fileName.contains(ScriptInfo::format) && !scriptList.contains(fileName)){
            addScript(fileName);
            loadScript(fileName);
        }
        else if(fileName.contains(SheetInfo::format) && !sheetList.contains(fileName)){
            addSheet(fileName);
            loadSheet(fileName);
        }
        else if(fileName == "." || fileName == "..")
            continue;
        else if(!otherList.contains(fileName) && !fileName.contains(ScriptInfo::format) && !fileName.contains(SheetInfo::format))
            addOther(fileName);
        else
            continue;
    }
}

void ReFileTree::setFolderPath(const QString &folderPath)
{
    dirWatcher->removePath(this->folderPath);
    dirWatcher->addPath(folderPath);
    this->folderPath = folderPath;
    loadFileTree();
}

void ReFileTree::pushClickedItem(QTreeWidgetItem *item, int column)
{
    /* フォルダーがクリックされた場合は無効 */
    if(item->parent() == nullptr) return;

    /* クリックされたファイルのフォルダー名(Script, Sheet, Other)を取得 */
    const QString folderName = item->parent()->text(column);
    const QString fileName = item->text(0);

    if(folderName == "Script")
        emit scriptSelected(fileName, scriptList.value(fileName));
    else if(folderName == "Sheet")
        emit sheetSelected(fileName, sheetList.value(fileName));
    else if(folderName == "Other")
        emit otherSelected(fileName, otherList.value(fileName));
}



void ReFileTree::onCustomContextMenu(const QPoint& point)
{
    if(selectedItems().count() < 1) return;

    //Widgetでのカーソル位置をグローバル座標に変換して渡す
    normalMenu->exec(viewport()->mapToGlobal(point));
}

void ReFileTree::addScript(const QString& fileName)
{
    /* ツリーへ追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(scriptTree);
    item->setText(0, fileName);

    /* リストへの追加 */
    scriptList.insert(fileName, ScriptInfo(new QProcess(), new ReTextEdit()));
}

void ReFileTree::addSheet(const QString& fileName)
{
    /* ツリーへ追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(sheetTree);
    item->setText(0, fileName);

    /* リストへの追加 */
    sheetList.insert(fileName, SheetInfo(new ReTableWidget()));
}

void ReFileTree::addOther(const QString& fileName)
{
    /* ツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(otherTree);
    item->setText(0, fileName);

    /* リストへの追加 */
    otherList.insert(fileName, OtherInfo());
}

bool ReFileTree::loadScript(const QString& fileName)
{
    const QString text = readFileTxt(folderPath + fileName);

    if(text == "\0") return false;

    scriptList.value(fileName).editor->setPlainText(text);

    return true;
}

bool ReFileTree::loadAllScript()
{
    const QStringList list = scriptList.keys();
    for(const QString& fileName : list)
    {
        const bool success = loadScript(fileName);
        if(!success) return false;
    }

    return true;
}

bool ReFileTree::saveScript(const QString& fileName)
{
    if(!scriptList.contains(fileName)) return false;

    return toFileTxt(folderPath + fileName, scriptList.value(fileName).editor->toPlainText());
}

bool ReFileTree::saveAllScript()
{
    const QStringList list = scriptList.keys();
    for(const QString& fileName : list)
    {
        const bool success = saveScript(fileName);
        if(!success) return false;
    }

    return true;
}

bool ReFileTree::loadSheet(const QString& fileName)
{
    sheetList.value(fileName).table->setData<QString>(readFileCsv(folderPath + fileName));

    return true;
}

bool ReFileTree::loadAllSheet()
{
    const QStringList list = sheetList.keys();
    for(const QString& fileName : list)
        loadSheet(fileName);

    return true;
}

bool ReFileTree::saveSheet(const QString& fileName)
{
    if(!sheetList.contains(fileName)) return false;

    return toFileCsv(folderPath + fileName, sheetList.value(fileName).table->getData<QString>());
}

bool ReFileTree::saveAllSheet()
{
    const QStringList list = sheetList.keys();
    for(const QString& fileName : list)
    {
        const bool success = saveSheet(fileName);
        if(!success) return false;
    }

    return true;
}






















