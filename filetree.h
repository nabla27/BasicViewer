#ifndef FILETREE_H
#define FILETREE_H

#include <QTreeWidget>
#include <QFileSystemWatcher>
#include <QProcess>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "basicset.h"

class FileTree : public QTreeWidget
{
public:
    FileTree(QWidget *parent = nullptr);
    ~FileTree();
public:
    QString currentScriptName() const { return (currentScriptIndex >= 0) ? scriptTree->child(currentScriptIndex)->text(0) : "\0"; }
    QString currentSheetName() const { return (currentSheetIndex >= 0) ? sheetTree->child(currentSheetIndex)->text(0) : "\0"; }
    QString currentOtherName() const { return (currentOtherIndex >= 0) ? otherTree->child(currentOtherIndex)->text(0) : "\0"; }
    QProcess *currentScriptProcess() const { return (currentScriptIndex >= 0) ? scriptList.value(currentScriptName()) : nullptr; }
    int scriptIndex() const { return currentScriptIndex; }
    int sheetIndex() const { return currentSheetIndex; }
    int otherIndex() const { return currentOtherIndex; }
    void reloadFileTree();                                  //FileTreeの更新
    void updateFileTree();                                  //ファイルの追加更新
    QStringList filenameList(const QString str = "") const; //全ファイル名を取得。リストから参照(リストからではない)
    void indexChange(QTreeWidgetItem *item, int column);
private:
    void addScriptFile(const QString& filename);  //scriptをTreeとListに追加
    void addSheetFile(const QString& filename);   //sheetをTreeとListに追加
    void addOtherFile(const QString& filename);   //otherをTreeとListに追加
private slots:
    /* 右クリック時のイベント */
    void onCustomContextMenu(const QPoint& point);
    /* contextMenu選択時のイベント */
    void addFile();
    void newFile();
    void renameFile();
    void removeFile();
    void saveFile();
private:
    QMenu *normalMenu;
    QFileSystemWatcher *watchDir;
    QTreeWidgetItem *scriptTree;
    QTreeWidgetItem *sheetTree;
    QTreeWidgetItem *otherTree;
    QMap<QString, QProcess*> scriptList;
    QSet<QString> sheetList;
    QSet<QString> otherList;

    int currentScriptIndex = -1;
    int currentSheetIndex = -1;
    int currentOtherIndex = -1;
};

#endif // FILETREE_H
