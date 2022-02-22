#ifndef RECMDLINE_H
#define RECMDLINE_H

#include <QLineEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>

class ReCmdLine : public QLineEdit
{
public:
    ReCmdLine(QWidget *parent = nullptr);
    ~ReCmdLine();
public:
    void setCompleter(QCompleter *c);
    QCompleter *completer() const { return c; }
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
private:
    void insertCompletion(const QString& completion);
    QString textUnderCursor() const;
private:
    QCompleter *c = nullptr;
};








QList<QString> ReparseCmdLine(const QString& cmd);


#endif // RECMDLINE_H
