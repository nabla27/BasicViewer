#ifndef GNUPLOT_H
#define GNUPLOT_H

#include <QProcess>
#include <QList>
#include <QString>
#include <QScrollBar>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QDir>
#include "textbrowser.h"
#include "basicset.h"


class Gnuplot
{
public:
    Gnuplot();
public:
    void exc(QProcess *process, const QList<QString>& cmdlist);
    void setOutBrowser(TextBrowser *output) { this->output = output; }
    void setExePath(const QString& path) { this->path = path; }
    void setPreCmd(const QString& preCmd) { this->preCmdList = preCmd.split('\n'); }
    void setOptionCmd(const QString& optionCmd) { this->optionCmd = optionCmd; }
    QString getOptionCmd() const { return optionCmd; }
    int getErrorLineNumber() const { return errorLineNumber; }
private:
    TextBrowser *output;
    QString path = "gnuplot.exe";
    QString optionCmd = "with linespoints";
    QList<QString> initCmdList;
    QList<QString> preCmdList;
    int errorLineNumber = -1;
};

#endif // GNUPLOT_H
