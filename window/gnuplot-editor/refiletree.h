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
#include "utility.h"

class TreeScriptItem : public QTreeWidgetItem
{
    //Q_OBJECT

public:
    TreeScriptItem(QTreeWidgetItem *parent, const QString& name);
    ~TreeScriptItem();

public:
    static const QString getFormat() { return format; }
    static void setFormat(const QString& _format) { format = _format; }

private:
    static QString format;
    QProcess *gnuplotProcess;
};



class TreeSheetItem : public QTreeWidgetItem
{
public:
    TreeSheetItem(QTreeWidgetItem *parent, const QString& name);
    ~TreeSheetItem();

public:
    static const QString getFormat() { return format; }
    static void setFormat(const QString& _format) { format = _format; }

private:
    static QString format;
};



class TreeOtherItem : public QTreeWidgetItem
{
public:
    TreeOtherItem(QTreeWidgetItem *parent, const QString& name);

public:
    const QString getFormat() { return format; }

private:
    QString format;
};





class ReFileTree : public QTreeWidget
{
    //Q_OBJECT

public:
    ReFileTree();


};

#endif // FILETREE_H
