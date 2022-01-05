#ifndef CMDLINE_H
#define CMDLINE_H

#include <QLineEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>

class CmdLine : public QLineEdit
{
public:
    CmdLine(QWidget *parent = nullptr);
    ~CmdLine();
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

#endif // CMDLINE_H
