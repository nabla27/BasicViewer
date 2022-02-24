#include "menubar.h"


FileMenu::FileMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *const importFolder = new QAction("import folder", this);
    addAction(importFolder);

    QAction *const importFile = new QAction("import file", this);
    addAction(importFile);

    QAction *const openFolder = new QAction("open folder", this);
    addAction(openFolder);

    QAction *const removeAllFile = new QAction("remove all file", this);
    addAction(removeAllFile);

    QAction *const reloadFolder = new QAction("reload folder", this);
    addAction(reloadFolder);
}

void FileMenu::importFolderAction()
{

}

void FileMenu::importFileAction()
{

}

void FileMenu::openFolderAction()
{

}

void FileMenu::removeAddFileAction()
{

}

void FileMenu::reloadFolderAction()
{

}






EditorMenu::EditorMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{

}





HelpMenu::HelpMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{

}






ScriptMenu::ScriptMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *const closeProcess = new QAction("Close this process", this);
    addAction(closeProcess);
}






SheetMenu::SheetMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{

}










































