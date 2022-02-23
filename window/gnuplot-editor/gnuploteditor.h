#ifndef GNUPLOTEDITOR_H
#define GNUPLOTEDITOR_H
#include <QMainWindow>
#include <QMenuBar>
#include <QScreen>
#include <QVBoxLayout>
#include <QPalette>
#include "menubar.h"
#include "utility.h"
#include "refiletree.h"
#include "retexteditor.h"

class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(QWidget *parent = nullptr);

private:
    void initializeMenuBar();
    void initializeLayout();

private:
    ReFileTree *fileTree;
    QTabWidget *editorTab;
    QTabWidget *displayTab;
    QWidget *gnuplotWidget;
    QWidget *sheetWidget;
    QWidget *consoleWidget;
    QWidget *stdoutWidget;

signals:

};

#endif // GNUPLOTEDITOR_H
