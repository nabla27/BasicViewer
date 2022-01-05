#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>
#include <QCompleter>
#include <QAbstractItemView>
#include <QTextBlock>
#include <QPainter>
#include <QPalette>
#include <QDir>
#include "syntaxhighlighter.h"
#include "completer/cpl_gnuplot.h"
#include "basicset.h"

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = nullptr);
    ~TextEdit();

public slots:
    void changeTabSpace(const QString& arg) { setTabStopDistance(arg.toDouble()); }
    void changeWrapMode(int arg) { setLineWrapMode(QPlainTextEdit::LineWrapMode(arg)); }
private:
    SyntaxHighlighter *textHighlight;

    /* completer */
public:
    void setCompleter(QCompleter *c);
    QCompleter *completer() const;
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
private:
    void insertCompletion(const QString& completion);           //予測変換で決定された文字をエディタに挿入
    QString textUnderCursor() const;                            //予測変換を出すために参照するテキスト
    void bracketCompletion(QKeyEvent *e, const QChar nextChar); //括弧の補完 [ ( ' "
    void changeCompleterModel();                                //入力コマンドから予測変換候補を変更
private:
    QCompleter *c = nullptr;
    int cursorMoveCount = 0;
    QString firstCmd = "";
    QString beforeCmd = "";
    QString currentCmd = "";

    /* lineNumer */
public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setErrorLineNumber(const int num) { errorLineNumber = num; }
public slots:
    void highlightLine();
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
private:
    QWidget *lineNumberArea;
    int errorLineNumber = -1;
};






class LineNumberArea : public QWidget
{
public:
    LineNumberArea(TextEdit *editor) : QWidget(editor), codeEditor(editor) {}
    ~LineNumberArea() {}
public:
    QSize sizeHint() const override{
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event) override{
        codeEditor->lineNumberAreaPaintEvent(event);
    }
private:
    TextEdit *codeEditor;
};


#endif // TEXTEDIT_H
