#include "refiletree.h"

QString ScriptList::format = ".txt";
void ScriptList::addScript(QTreeWidgetItem *parent, const QString& fileName)
{
    /* ファイルツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, fileName);

    /* リストへの追加 */
    fileList.insert(fileName, new ScriptInfo(new QProcess(), new ReTextEdit()));
}

bool ScriptList::loadScript(const QString &folderPath, const QString &fileName)
{
    const QString text = readFileTxt(folderPath + fileName);

    if(text == "\0") return false;

    fileList.value(fileName)->textEditor->setPlainText(text);

    return true;
}

bool ScriptList::loadAllScript(const QString &folderPath)
{
    const QList<QString> list = fileList.keys();
    for(const QString& fileName : list)
    {
        const bool success = loadScript(folderPath, fileName);
        if(!success) return false;
    }

    return true;
}

bool ScriptList::saveScript(const QString &folderPath, const QString &fileName)
{
    if(!fileList.contains(fileName)) return false;

    return toFileTxt(folderPath + fileName, fileList.value(fileName)->textEditor->toPlainText());
}

bool ScriptList::saveAllScript(const QString &folderPath)
{
    const QList<QString> list = fileList.keys();
    for(const QString& fileName : list)
    {
        const bool success = saveScript(folderPath, fileName);
        if(!success) return false;
    }

    return true;
}







QString SheetList::format = ".csv";
void SheetList::addSheet(QTreeWidgetItem *parent, const QString &fileName)
{
    /* ファイルツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, fileName);

    /* リストへの追加 */
    fileList.insert(fileName, new ReTableWidget());
}

bool SheetList::loadSheet(const QString &folderPath, const QString &fileName)
{
    fileList.value(fileName)->setData<QString>(readFileCsv(folderPath + fileName));

    return true;
}

bool SheetList::loadAllSheet(const QString &folderPath)
{
    const QList<QString> list = fileList.keys();
    for(const QString& fileName : list)
        loadSheet(folderPath, fileName);

    return true;
}

bool SheetList::saveSheet(const QString &folderPath, const QString &fileName)
{
    if(!fileList.contains(fileName)) return false;

    return toFileCsv(folderPath + fileName, fileList.value(fileName)->getData<QString>());
}

bool SheetList::saveAllSheet(const QString &folderPath)
{
    const QList<QString> list = fileList.keys();
    for(const QString& fileName : list)
    {
        const bool success = saveSheet(folderPath, fileName);
        if(!success) return false;
    }

    return true;
}












void OtherList::addOther(QTreeWidgetItem *parent, const QString &fileName)
{
    /* ファイルツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, fileName);

    /* リストへの追加 */
    fileList.insert(fileName);
}












ReFileTree::ReFileTree(QWidget *parent)
    : QTreeWidget(parent), scriptList(parent), sheetList(parent), otherList(parent)
{
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

        if(fileName.contains(scriptList.format) && !scriptList.isContains(fileName)){
            scriptList.addScript(scriptTree, fileName);
            scriptList.loadScript(folderPath, fileName);
        }
        else if(fileName.contains(sheetList.format) && !sheetList.isContains(fileName)){
            sheetList.addSheet(sheetTree, fileName);
            sheetList.loadSheet(folderPath, fileName);
        }
        else if(fileName == "." || fileName == "..")
            continue;
        else if(!otherList.isContains(fileName))
            otherList.addOther(otherTree, fileName);
        else
            continue;
    }
}

void ReFileTree::pushClickedItem(QTreeWidgetItem *item, int column)
{
    /* フォルダーがクリックされた場合は無効 */
    if(item->parent() == nullptr) return;

    /* クリックされたファイルのフォルダー名(Script, Sheet, Other)を取得 */
    const QString folderName = item->parent()->text(column);
    const QString fileName = item->text(0);

    if(folderName == "Script")
    {
        emit scriptSelected(fileName,
                            scriptList.getScriptInfo(fileName)->textEditor,
                            scriptList.getScriptInfo(fileName)->process);
    }
    else if(folderName == "Sheet")
    {
        emit sheetSelected(fileName,
                           sheetList.getSheet(fileName));
    }
}























