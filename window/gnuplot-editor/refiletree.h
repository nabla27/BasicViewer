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
#include "utility.h"
#include "retexteditor.h"
#include "retablewidget.h"

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
    bool isContains(const QString& fileName) { return scriptList.contains(fileName); }
    ScriptInfo* getScriptInfo(const QString& fileName) { return scriptList.value(fileName); }
    static QString format;

private:
    QWidget *parentWidget;
    QHash<QString, ScriptInfo*> scriptList;
};


class SheetList : public QObject
{
    Q_OBJECT

public:
    SheetList(QObject *parent) : QObject(parent) {}

public:
    void addSheet(QTreeWidgetItem *parent, const QString& fileName);
    bool isContains(const QString& fileName) { return sheetList.contains(fileName); }
    ReTableWidget* getSheet(const QString& fileName) const { return sheetList.value(fileName); }
    static QString format;

private:
    QWidget *parentWidget;
    QHash<QString, ReTableWidget*> sheetList;
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
    void getClickedItem(QTreeWidgetItem *item, int column);

signals:
    void scriptSelected(ReTextEdit *editor, QProcess *process);
    void sheetSelected(ReTableWidget *sheet);

private:
    const QString folderPath = BasicSet::tmpDirectory;
    ScriptList scriptList;
    SheetList sheetList;
    QTreeWidgetItem *scriptTree;
    QTreeWidgetItem *sheetTree;
    QTreeWidgetItem *otherTree;
};

#endif // FILETREE_H
