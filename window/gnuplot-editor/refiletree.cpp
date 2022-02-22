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






ReFileTree::ReFileTree()
{

}
