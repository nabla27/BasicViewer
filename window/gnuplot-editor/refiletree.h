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
    ~ScriptInfo() { delete process; process = nullptr; delete editor; editor = nullptr; }

    static bool isValidFormat(const QString& fileName){
        for(const QString& format : formatList) if(fileName.contains(format)) return true;
        return false;
    }

    QProcess *process;
    ReTextEdit *editor;
    static QStringList formatList;
};

struct SheetInfo
{
public:
    SheetInfo(ReTableWidget *table = nullptr) : table(table) {}
    ~SheetInfo() { delete table; table = nullptr; }

    static bool isValidFormat(const QString& fileName){
        for(const QString& format : formatList) if(fileName.contains(format)) return true;
        return false;
    }

    ReTableWidget *table;
    static QStringList formatList;
};

struct OtherInfo
{
public:
    OtherInfo() {}
    ~OtherInfo() {}
};














class ReFileTree : public QTreeWidget
{
    Q_OBJECT

public:
    ReFileTree(QWidget *parent);
    ~ReFileTree();

public slots:
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

    void addFile();
    void newFile();
    void renameFile();
    void removeFile();
    void exportFile();

private:
    void initializeContextMenu();
    bool containsFile(const QString& fileName) const { return scriptList.contains(fileName) || sheetList.contains(fileName) || otherList.contains(fileName); }

private slots:
    void pushClickedItem(QTreeWidgetItem *item, int column);
    void onCustomContextMenu(const QPoint& point);

private:
    QString folderPath = QDir::currentPath() + "/" + BasicSet::tmpDirectory;
    QTreeWidgetItem *scriptTree;
    QTreeWidgetItem *sheetTree;
    QTreeWidgetItem *otherTree;
    QFileSystemWatcher *dirWatcher;
    QHash<QString, ScriptInfo*> scriptList;
    QHash<QString, SheetInfo*> sheetList;
    QHash<QString, OtherInfo*> otherList;

    QMenu *normalMenu;

signals:
    void scriptSelected(const QString& fileName, const ScriptInfo* info);
    void sheetSelected(const QString& fileName, const SheetInfo* info);
    void otherSelected(const QString& fileName, const OtherInfo* info);
    void fileNameChanged(const QString& oldName, const QString newName);
    void scriptRemoved(const QString& fileName, const ScriptInfo* info);
    void sheetRemoved(const QString& fileName, const SheetInfo* info);
    void otherRemoved(const QString& fileName, const OtherInfo* info);
};

#endif // FILETREE_H
