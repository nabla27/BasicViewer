#ifndef IOFILE_H
#define IOFILE_H

#include <QString>
#include <QFile>
#include <QTextStream>

bool toFileTxt(const QString& fileName, const QString& data)
{
    QFile file(fileName);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << data;
        file.close();
        return true;
    }

    return false;
}

bool toFileCsv(const QString& filename, const QList<QList<QString> >& sheet)
{
    QString data;
    for(qsizetype row = 0; row < sheet.size(); ++row){
        for(qsizetype col = 0; col < sheet.at(row).size(); ++col)
        {
            data += sheet.at(row).at(col);
            if(col != sheet.at(row).size() - 1) { data += ","; }
        }
        if(row != sheet.size() - 1) { data += "\n"; }
    }

    return toFileTxt(filename, data);
}

QString readFileTxt(const QString& fileName)
{
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        const QString data = in.readAll();
        return data;
    }

    return "\0";
}

QList<QList<QString> > readFileCsv(const QString& fileName)
{
    const QString data = readFileTxt(fileName);
    QList<QList<QString> > sheet(1);
    qsizetype row = 0;
    QString stack = "";

    for(const QChar& c : data)
    {
        if(c == '\n'){
            sheet.append(QList<QString>(0));
            row++;
        }
        else if(c == ','){
            sheet[row] << stack;
            stack.clear();
        }
        else
            stack += c;
    }

    return sheet;
}

#endif // IOFILE_H
