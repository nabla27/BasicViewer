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





















#endif // MENUBAR_H
