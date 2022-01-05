#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QTextBrowser>

class TextBrowser : public QTextBrowser
{
public:
    TextBrowser(QWidget *parent = nullptr);
public:
    void outMessage(const QString& message, const QString& type = "unknown");
private:
    QString messageType = "";
};

#endif // TEXTBROWSER_H
