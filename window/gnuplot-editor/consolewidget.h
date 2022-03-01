#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>

class ConsoleWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    ConsoleWidget(QWidget *parent);

public:
    void setCompleter(QCompleter *c);
    QCompleter* completer() const;

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private:
    void insertCompletion(const QString& completion);
    QString textUnderCursor() const;

private:
    QCompleter *c = nullptr;
};

#endif // CONSOLEWIDGET_H
