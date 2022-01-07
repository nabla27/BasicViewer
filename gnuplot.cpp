#include "gnuplot.h"

Gnuplot::Gnuplot()
{
    initCmdList << "cd '" + QDir::currentPath() + "/" + BasicSet::tmpDirectory;
    initCmdList << "set datafile separator ','";
}

void Gnuplot::exc(QProcess *process, const QList<QString>& cmdlist)
{
    if(process == nullptr) { return; }

    errorLineNumber = -1;  //エラー行。戻り値。エラーなしの0にリセット

    /* 標準出力 */
    QObject::connect(process, &QProcess::readyReadStandardOutput, [process, this](){
        const QString output = process->readAllStandardOutput();
        if(this->output != nullptr){
            this->output->outMessage(output, "gnuplot");
            this->output->verticalScrollBar()->setValue(this->output->verticalScrollBar()->maximum());
        }
    });

    /* 標準エラー */
    QObject::connect(process, &QProcess::readyReadStandardError, [process, this](){
        const QString err = process->readAllStandardError();
        if(err.isEmpty()) return;
        if(this->output != nullptr){
            this->output->outMessage(err, "gnuplot");
            this->output->verticalScrollBar()->setValue(this->output->verticalScrollBar()->maximum());
        }

        /* エラー行の取得 */
        QList<int> list;  //エラー行を格納するリスト
        QRegularExpressionMatchIterator iter = QRegularExpression("line \\d+:").globalMatch(err);
        while(iter.hasNext()){
            QRegularExpressionMatch match = iter.next();
            list << match.captured(0).sliced(5, match.captured(0).size() - 6).toInt() - 1;
        }
        errorLineNumber = (list.size() < 1) ? 0 : list.at(0);
    });

    /* プロセスの開始 */
    if(process->state() == QProcess::ProcessState::NotRunning)
    {
        process->start(path, QStringList() << "-persist");
        if(process->error() == QProcess::ProcessError::FailedToStart){
            process->close();
            this->output->outMessage("failed to start a process. check the path of setting.", "gnuplot");
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
        process->write(cmd.toUtf8().constData());
}
