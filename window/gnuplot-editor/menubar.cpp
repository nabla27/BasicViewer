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
