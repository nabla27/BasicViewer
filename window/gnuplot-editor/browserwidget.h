#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H
#include <QTextBrowser>
#include <QSyntaxHighlighter>
#include "utility.h"

class BrowserSyntaxHighlighter;

class BrowserWidget : public QTextBrowser
{
    Q_OBJECT

public:
    explicit BrowserWidget(QWidget *parent = nullptr);
    ~BrowserWidget();

    enum class MessageType { GnuplotStdOut,
                             GnuplotStdErr,
                             FileSystemErr,
                             ProcessErr,
                             SystemErr,
                             Unknown };
    Q_ENUM(MessageType)

public:
    void outputText(const QString& text, const MessageType messageType);

private:
    BrowserSyntaxHighlighter *highlighter;
};








class BrowserSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit BrowserSyntaxHighlighter(QTextDocument *document);

public:
    void highlightBlock(const QString& text) override;

private:
    BrowserWidget::MessageType messageType = BrowserWidget::MessageType::Unknown;
};

#endif // BROWSERWIDGET_H
