#ifndef MENUBAR_H
#define MENUBAR_H
#include <QMenu>
#include <QAction>

class FileMenu : public QMenu
{
    Q_OBJECT

public:
    FileMenu(const QString& title, QWidget *parent);

private slots:
    void importFolderAction();
    void importFileAction();
    void openFolderAction();
    void removeAddFileAction();
    void reloadFolderAction();

signals:
};


class EditorMenu : public QMenu
{
    Q_OBJECT

public:
    EditorMenu(const QString& title, QWidget *parent);
};


class HelpMenu : public QMenu
{
    Q_OBJECT

public:
    HelpMenu(const QString& title, QWidget *parent);
};


class ScriptMenu : public QMenu
{
    Q_OBJECT

public:
    ScriptMenu(const QString& title, QWidget *parent);
};


class SheetMenu : public QMenu
{
    Q_OBJECT

public:
    SheetMenu(const QString& title, QWidget *parent);
};

















#endif // MENUBAR_H
