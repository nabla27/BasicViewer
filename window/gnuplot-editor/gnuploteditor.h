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
#include "retextbrowser.h"

class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(QWidget *parent = nullptr);

private:
    void initializeMenuBar();
    void initializeLayout();

private slots:
    void setEditorWidget(ReTextEdit *editor, QProcess *process);
    void setSheetWidget(ReTableWidget *sheet);

private:
    ReFileTree *fileTree;
    QTabWidget *editorTab;
    QTabWidget *displayTab;
    QStackedWidget *gnuplotWidget;
    QStackedWidget *sheetWidget;
    QWidget *consoleWidget;
    QWidget *outputWidget;
    QProcess *gnuplotProcess;

signals:

};

#endif // GNUPLOTEDITOR_H
