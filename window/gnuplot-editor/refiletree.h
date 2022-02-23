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
#include "utility.h"
#include "retexteditor.h"
#include "retablewidget.h"
#include "io/iofile.h"

class ScriptList : public QObject
{
    Q_OBJECT

public:
    ScriptList(QWidget *parent) : QObject(parent), parentWidget(parent) {}

    class ScriptInfo{
    public:
        ScriptInfo(QProcess *process, ReTextEdit *textEditor)
            : process(process), textEditor(textEditor) {}
        QProcess *process;
        ReTextEdit *textEditor;
    };

public:
    void addScript(QTreeWidgetItem *parent, const QString& fileName);
    bool isContains(const QString& fileName) { return fileList.contains(fileName); }
    ScriptInfo* getScriptInfo(const QString& fileName) { return fileList.value(fileName); }
    bool loadScript(const QString& folderPath, const QString& fileName);
    bool loadAllScript(const QString& folderPath);
    bool saveScript(const QString& folderPath, const QString& fileName);
    bool saveAllScript(const QString& folderPath);
    static QString format;

private:
    QWidget *parentWidget;
    QHash<QString, ScriptInfo*> fileList;
};







class SheetList : public QObject
{
    Q_OBJECT

public:
    SheetList(QWidget *parent) : QObject(parent), parentWidget(parent) {}

public:
    void addSheet(QTreeWidgetItem *parent, const QString& fileName);
    bool isContains(const QString& fileName) { return fileList.contains(fileName); }
    ReTableWidget* getSheet(const QString& fileName) const { return fileList.value(fileName); }
    bool loadSheet(const QString& folderPath, const QString& fileName);
    bool loadAllSheet(const QString& folderPath);
    bool saveSheet(const QString& folderPath, const QString& fileName);
    bool saveAllSheet(const QString& folderPath);
    static QString format;

private:
    QWidget *parentWidget;
    QHash<QString, ReTableWidget*> fileList;
};







class OtherList : public QObject
{
    Q_OBJECT

public:
    OtherList(QObject *parent) : QObject(parent) {}

public:
    void addOther(QTreeWidgetItem *parent, const QString& fileName);
    bool isContains(const QString& fileName) { return fileList.contains(fileName); }

private:
    QSet<QString> fileList;
};






class CustomList : public QObject
{
    Q_OBJECT

public:
    CustomList(QObject *parent, const QString& format) : QObject(parent), format(format) {}

public:
    void addCustom(QTreeWidgetItem *parent, const QString& fileName);
    bool isContains(const QString& fileName) { return fileList.contains(fileName); }
    QString format;

private:
    QSet<QString> fileList;
};





















class ReFileTree : public QTreeWidget
{
    Q_OBJECT

public:
    ReFileTree(QWidget *parent);

private:
    void loadFileTree();
    void updateFileTree();

private slots:
    void pushClickedItem(QTreeWidgetItem *item, int column);

signals:
    void scriptSelected(const QString& fileName, ReTextEdit *editor, QProcess *process);
    void sheetSelected(const QString& fileName, ReTableWidget *sheet);

private:
    const QString folderPath = BasicSet::tmpDirectory;
    ScriptList scriptList;
    SheetList sheetList;
    OtherList otherList;
    QTreeWidgetItem *scriptTree;
    QTreeWidgetItem *sheetTree;
    QTreeWidgetItem *otherTree;
};

#endif // FILETREE_H
