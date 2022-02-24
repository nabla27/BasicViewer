#include "browserwidget.h"


BrowserSyntaxHighlighter::BrowserSyntaxHighlighter(QTextDocument *document)
    : QSyntaxHighlighter(document)
{

}

void BrowserSyntaxHighlighter::highlightBlock(const QString &text)
{
    if(text.isEmpty()) return;

    /* ハイライト */
}













BrowserWidget::BrowserWidget(QWidget *parent)
    : QTextBrowser(parent)
{
    highlighter = new BrowserSyntaxHighlighter(document());
}

BrowserWidget::~BrowserWidget()
{
    delete highlighter;
}

void BrowserWidget::outputText(const QString &text, const MessageType messageType)
{
    insertPlainText("[ " + enumToString(messageType) + " ]\n");
    insertPlainText(text);
}











































