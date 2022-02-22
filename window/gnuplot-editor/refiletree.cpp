#include "refiletree.h"


void ScriptList::addScript(QTreeWidgetItem *parent, const QString& fileName)
{
    /* ファイルツリーへの追加 */
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, fileName);

    /* リストへの追加 */
    scriptList.insert(fileName, new ScriptInfo(new QProcess()));
}












ReFileTree::ReFileTree()
{

}

void ReFileTree::updateFileTree()
{
    /* ディレクトリのファイル情報を取得 */
    QDir dir(folderPath);
    QFileInfoList fileList = dir.entryInfoList();

    /* ファイルを順に捜査して、拡張子ごとにフォルダーに分ける。 */

}

























