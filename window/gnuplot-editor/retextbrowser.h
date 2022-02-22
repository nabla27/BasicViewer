#ifndef RETEXTBROWSER_H
#define RETEXTBROWSER_H
#include <QWidget>

class ReTextBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit ReTextBrowser(QWidget *parent = nullptr);

public:
    void outMessage(const QString& message, const QString& type = "unknown") {}
    void setScrollBarMaximum() {}

private:
    QString messagetType = "";

signals:
};

#endif // RETEXTBROWSER_H
