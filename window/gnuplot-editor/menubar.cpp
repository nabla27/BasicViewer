#include "menubar.h"


FileMenu::FileMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{
    QAction *importFolder = new QAction("import folder", this);
    addAction(importFolder);

    QAction *importFile = new QAction("import file", this);
    addAction(importFile);

    QAction *openFolder = new QAction("open folder", this);
    addAction(openFolder);

    QAction *removeAllFile = new QAction("remove all file", this);
    addAction(removeAllFile);

    QAction *reloadFolder = new QAction("reload folder", this);
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
    QAction *closeProcess = new QAction("Close this process", this);
    addAction(closeProcess);
}






ScriptMenu::ScriptMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{

}






SheetMenu::SheetMenu(const QString& title, QWidget *parent)
    : QMenu(title, parent)
{

}










































