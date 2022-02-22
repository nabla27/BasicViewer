#ifndef GNUPLOTEDITOR_H
#define GNUPLOTEDITOR_H
#include <QMainWindow>
#include <QMenuBar>
#include <QScreen>
#include "menubar.h"
#include "utility.h"

class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(QWidget *parent = nullptr);

private:
    void initializeMenuBar();

signals:

};

#endif // GNUPLOTEDITOR_H
