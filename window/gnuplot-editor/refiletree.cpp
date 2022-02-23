#include "refiletree.h"

QString ScriptList::format = ".txt";
void ScriptList::addScript(QTreeWidgetItem *parent, const QString& fileName)
{
    /* ファイルツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, fileName);

    /* リストへの追加 */
    scriptList.insert(fileName, new ScriptInfo(new QProcess(), new ReTextEdit()));
}


QString SheetList::format = ".csv";
void SheetList::addSheet(QTreeWidgetItem *parent, const QString &fileName)
{
    /* ファイルツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, fileName);

    /* リストへの追加 */
    sheetList.insert(fileName, new ReTableWidget());
}












ReFileTree::ReFileTree(QWidget *parent)
    : QTreeWidget(parent), scriptList(parent), sheetList(parent)
{
    loadFileTree();

    connect(this, &ReFileTree::itemDoubleClicked, this, &ReFileTree::getClickedItem);
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

        if(fileName.contains(scriptList.format) && !scriptList.isContains(fileName))
            scriptList.addScript(scriptTree, fileName);
        else if(fileName.contains(sheetList.format) && !sheetList.isContains(fileName))
            sheetList.addSheet(sheetTree, fileName);
        else if(fileName == "." || fileName == "..")
            continue;
        else
            continue;
    }
}

void ReFileTree::getClickedItem(QTreeWidgetItem *item, int column)
{
    /* フォルダーがクリックされた場合は無効 */
    if(item->parent() == nullptr) return;

    /* クリックされたがファイルのフォルダー名(Script, Sheet, Other)を取得 */
    const QString folderName = item->parent()->text(column);
    const QString fileName = item->text(0);

    if(folderName == "Script")
    {
        emit scriptSelected(scriptList.getScriptInfo(fileName)->textEditor,
                            scriptList.getScriptInfo(fileName)->process);
    }
    else if(folderName == "Sheet")
    {
        emit sheetSelected(sheetList.getSheet(fileName));
    }
}























