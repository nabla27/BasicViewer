#ifndef REFILETREE_H
#define REFILETREE_H

#include <QTreeWidget>
#include <QFileSystemWatcher>
#include <QProcess>
#include <QMenu>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QHash>
#include <QSet>
#include <QDir>
#include "utility.h"
#include "retexteditor.h"
#include "retablewidget.h"
#include "io/iofile.h"






struct ScriptInfo
{
public:
    ScriptInfo(QProcess *process = nullptr, ReTextEdit *editor = nullptr)
        : process(process), editor(editor) {}
    QProcess *process;
    ReTextEdit *editor;
    static QString format;
};

struct SheetInfo
{
public:
    SheetInfo(ReTableWidget *table = nullptr) : table(table) {}
    ReTableWidget *table;
    static QString format;
};

struct OtherInfo
{
public:
    OtherInfo() {}
};














class ReFileTree : public QTreeWidget
{
    Q_OBJECT

public:
    ReFileTree(QWidget *parent);

public:
    void loadFileTree();
    void updateFileTree();
    void setFolderPath(const QString& folderPath);
    void addScript(const QString& fileName);
    void addSheet(const QString& fileName);
    void addOther(const QString& fileName);
    bool loadScript(const QString& fileName);
    bool saveScript(const QString& fileName);
    bool loadSheet(const QString& sheetName);
    bool saveSheet(const QString& sheetName);
    bool loadAllScript();
    bool saveAllScript();
    bool loadAllSheet();
    bool saveAllSheet();

private slots:
    void pushClickedItem(QTreeWidgetItem *item, int column);
    void onCustomContextMenu(const QPoint& point);

private:
    QString folderPath = QDir::currentPath() + "/" + BasicSet::tmpDirectory;
    QTreeWidgetItem *scriptTree;
    QTreeWidgetItem *sheetTree;
    QTreeWidgetItem *otherTree;
    QFileSystemWatcher *dirWatcher;
    QHash<QString, ScriptInfo> scriptList;
    QHash<QString, SheetInfo> sheetList;
    QHash<QString, OtherInfo> otherList;

    QMenu *normalMenu;

signals:
    void scriptSelected(const QString& fileName, const ScriptInfo& info);
    void sheetSelected(const QString& fileName, const SheetInfo& info);
    void otherSelected(const QString& fileName, const OtherInfo& info);
};

#endif // FILETREE_H
