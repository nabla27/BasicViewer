#ifndef GNUPLOTEDITOR_H
#define GNUPLOTEDITOR_H
#include <QMainWindow>
#include <QMenuBar>
#include "menubar.h"

class GnuplotEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit GnuplotEditor(QWidget *parent = nullptr);

signals:

};

#endif // GNUPLOTEDITOR_H
