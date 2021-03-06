#include "regnuplot.h"

ReGnuplot::ReGnuplot(QObject *parent)
    : QObject(parent)
{
    initCmdList << "cd '" + workingPath + "'";
    initCmdList << "set datafile separator ','";
}

void ReGnuplot::exc(QProcess *process, const QList<QString>& cmdlist)
{
    if(!process)  return;

    currentProcess = process;

    /* 標準出力 */
    QObject::connect(process, &QProcess::readyReadStandardOutput, this, &ReGnuplot::readStandardOutput);

    /* 標準エラー */
    QObject::connect(process, &QProcess::readyReadStandardError, this, &ReGnuplot::readStandardError);

    /* プロセスの開始 */
    if(process->state() == QProcess::ProcessState::NotRunning)
    {
        process->start(path, QStringList() << "-persist");
        if(process->error() == QProcess::ProcessError::FailedToStart){
            process->close();
            emit errorCaused("failed to start the gnuplot process.", BrowserWidget::MessageType::ProcessErr);
        }
    }

    /* 初期コマンドの実行 */
    for(const QString& initCmd : initCmdList)
        process->write((initCmd + "\n").toUtf8().constData());

    /* 事前コマンドの実行 */
    for(const QString& preCmd : preCmdList)
        process->write((preCmd + "\n").toUtf8().constData());

    /* コマンドの実行 */
    for(const QString& cmd : cmdlist)
        process->write((cmd + "\n").toUtf8().constData());
}

int ReGnuplot::getErrorLineNumber(const QString &err)
{
    /* エラー行の取得 */
    QList<int> list;  //エラー行を格納するリスト
    QRegularExpressionMatchIterator iter = QRegularExpression("line \\d+:").globalMatch(err);
    while(iter.hasNext()){
        QRegularExpressionMatch match = iter.next();
        list << match.captured(0).sliced(5, match.captured(0).size() - 6).toInt();
    }

    return (list.size() < 1) ? -1 : list.at(0);
}

void ReGnuplot::readStandardOutput()
{
    const QString output = currentProcess->readAllStandardOutput();

    if(output.isEmpty()) return;

    emit standardOutputPassed(output);
}

void ReGnuplot::readStandardError()
{
    const QString output = currentProcess->readAllStandardError();

    if(output.isEmpty()) return;

    const int errLine = getErrorLineNumber(output); //エラー行を示す表現が見つけられなかったら-1を返す

    if(errLine == -1)
        emit standardOutputPassed(output);          //エラーじゃない標準出力でもreadAllStandardError()で拾われてしまう
    else
        emit standardErrorPassed(output, errLine);
}


