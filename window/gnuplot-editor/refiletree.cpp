#include "refiletree.h"


QString TreeScriptItem::format = ".txt";
TreeScriptItem::TreeScriptItem(QTreeWidgetItem *parent, const QString& name)
    : QTreeWidgetItem(parent)
{
    setText(0, name);
    gnuplotProcess = new QProcess(nullptr);
}

TreeScriptItem::~TreeScriptItem()
{
    gnuplotProcess->close(); delete gnuplotProcess;
}

QString TreeSheetItem::format = ".csv";
TreeSheetItem::TreeSheetItem(QTreeWidgetItem *parent, const QString& name)
    : QTreeWidgetItem(parent)
{
    setText(0, name);
}

TreeSheetItem::~TreeSheetItem()
{

}


TreeOtherItem::TreeOtherItem(QTreeWidgetItem *parent, const QString& name)
    : QTreeWidgetItem(parent)
{
    setText(0, name);
    format =  name.sliced(name.lastIndexOf('.'));
}












ReFileTree::ReFileTree()
{

}
