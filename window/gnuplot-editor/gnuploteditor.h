#ifndef GNUPLOTEDITOR_H
#define GNUPLOTEDITOR_H
#include <QMainWindow>
#include <QMenuBar>
#include <QScreen>
#include <QVBoxLayout>
#include <QPalette>
#include <QStackedWidget>

#include "menubar.h"
#include "utility.h"
#include "refiletree.h"
#include "retexteditor.h"
#include "retablewidget.h"
#include "browserwidget.h"
#include "windowmenubar.h"
#include "subwindow/layoutparts.h"

class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(QWidget *parent = nullptr);

private:
    void initializeMenuBar();
    void initializeLayout();

private slots:
    void setEditorWidget(const QString& fileName, const ScriptInfo& info);
    void setSheetWidget(const QString& fileName, const SheetInfo& info);
    void setOtherWidget(const QString& fileName, const OtherInfo& info);
    void setFolderPath(const QString& folderPath);
    void executeGnuplot();
    void receiveGnuplotStdOut(const QString& text);
    void receiveGnuplotStdErr(const QString& text, const int line);
    void setFileTreeWidth(const int dx);
    void setDisplayTabHeight(const int dy);

private:
    ScriptMenu *scriptMenu;
    SheetMenu *sheetMenu;

    ReGnuplot *gnuplot;

    ReFileTree *fileTree;
    QTabWidget *editorTab;
    QTabWidget *displayTab;
    QStackedWidget *gnuplotWidget;
    QStackedWidget *sheetWidget;
    QWidget *consoleWidget;
    BrowserWidget *browserWidget;
    QProcess *gnuplotProcess;

signals:

};

#endif // GNUPLOTEDITOR_H
