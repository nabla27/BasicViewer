#ifndef RESYNTAXHIGHLIGHTER_H
#define RESYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QString>

class ReSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit ReSyntaxHighlighter(QTextDocument *documtent);
    virtual ~ReSyntaxHighlighter() {}

public:
    virtual void highlightBlock(const QString& text);
private:
    QTextCharFormat firstOrderFormat;
    QStringList firstOrderKeyWord;
    QTextCharFormat commentsFormat;
    QTextCharFormat quotationFormat;
};

#endif // RESYNTAXHIGHLIGHTER_H
