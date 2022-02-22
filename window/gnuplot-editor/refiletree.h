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

class TextEdit;

class ScriptList : public QObject
{
    Q_OBJECT

public:
    ScriptList(QObject *parent) : QObject(parent) {}

    class ScriptInfo{
    public:
        ScriptInfo(QProcess *process) : process(process) {}
        QProcess *process;
    };

public:
    void addScript(QTreeWidgetItem *parent, const QString& fileName);
    bool isContains(const QString& fileName) { return scriptList.contains(fileName); }
    ScriptInfo *const getScriptInfo(const QString& fileName) { return scriptList.value(fileName); }

private:
    QHash<QString, ScriptInfo*> scriptList;
};

class SheetList : public QObject
{
    Q_OBJECT

public:
    SheetList(QObject *parent) : QObject(parent) {}

public:

private:
};



















class ReFileTree : public QTreeWidget
{
    //Q_OBJECT

public:
    ReFileTree();

private:
    void updateFileTree();

private:
    const QString folderPath = BasicSet::tmpDirectory;
};

#endif // FILETREE_H
