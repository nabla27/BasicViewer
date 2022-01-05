#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QString>

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit SyntaxHighlighter(QTextDocument *documtent);
    virtual ~SyntaxHighlighter() {}

public:
    virtual void highlightBlock(const QString& text);
private:
    QTextCharFormat firstOrderFormat;
    QStringList firstOrderKeyWord;
    QTextCharFormat commentsFormat;
    QTextCharFormat quotationFormat;
};

#endif // SYNTAXHIGHLIGHTER_H
