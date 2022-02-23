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
    void setExePath(const QString& path) { this->path = path; }
    void setPreCmd(const QString& preCmd) { this->preCmdList = preCmd.split('\n'); }
    void setOptionCmd(const QString& optionCmd) { this->optionCmd = optionCmd; }

private:
    int getErrorLineNumber(const QString& err);

private slots:
    void readStandardOutput();
    void readStandardError();

private:
    QString path = "gnuplot.exe";
    QString optionCmd = "with linespoints";
    QList<QString> initCmdList;
    QList<QString> preCmdList;
    QProcess *currentProcess;

signals:
    void standardOutputPassed(const QString& out);
    void standardErrorPassed(const QString& out, const int line);
    void startProcessFailed();
};
#endif // REGNUPLOT_H
