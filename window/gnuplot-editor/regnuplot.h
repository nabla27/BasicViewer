#ifndef REGNUPLOT_H
#define REGNUPLOT_H


#include <QProcess>
#include <QList>
#include <QString>
#include <QScrollBar>
#include <QObject>
#include <QRegularExpressionMatchIterator>
#include <QDir>
#include "retextbrowser.h"
#include "utility.h"


class ReGnuplot
{
public:
    ReGnuplot();
public:
    void exc(QProcess *process, const QList<QString>& cmdlist);
    void setOutBrowser(ReTextBrowser *output) { this->output = output; }
    void setExePath(const QString& path) { this->path = path; }
    void setPreCmd(const QString& preCmd) { this->preCmdList = preCmd.split('\n'); }
    void setOptionCmd(const QString& optionCmd) { this->optionCmd = optionCmd; }
    QString getOptionCmd() const { return optionCmd; }
    int getErrorLineNumber() const { return errorLineNumber; }

private:
    ReTextBrowser *output;
    QString path = "gnuplot.exe";
    QString optionCmd = "with linespoints";
    QList<QString> initCmdList;
    QList<QString> preCmdList;
    int errorLineNumber = -1;
};
#endif // REGNUPLOT_H
