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
    void setFirstCmdColor(const QColor& color) { firstOrderFormat.setForeground(QColor(color)); rehighlight(); }
    void setCommentColor(const QColor& color) { commentsFormat.setForeground(QColor(color)); rehighlight(); }
    void setStringColor(const QColor& color) { quotationFormat.setForeground(QColor(color)); rehighlight(); }

private:
    QTextCharFormat firstOrderFormat;
    QStringList firstOrderKeyWord;
    QTextCharFormat commentsFormat;
    QTextCharFormat quotationFormat;
};

#endif // RESYNTAXHIGHLIGHTER_H
