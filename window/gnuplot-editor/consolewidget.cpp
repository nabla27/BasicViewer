#include "consolewidget.h"

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QPlainTextEdit(parent)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::GlobalColor::black);
    palette.setColor(QPalette::Text, Qt::GlobalColor::white);
    setPalette(palette);

    setFrameShape(QFrame::Shape::NoFrame);
}

void ConsoleWidget::setCompleter(QCompleter *completer)
{
    //新しくcompleterを設定
    if(c) c->disconnect(this);
    c = completer;

    if(!c) return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
}

QCompleter* ConsoleWidget::completer() const
{
    return c;
}

void ConsoleWidget::keyPressEvent(QKeyEvent *e)
{
    if(c && c->popup()->isVisible())
    {
        switch(e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); //ctrl + E
    if(!c || !isShortcut) QPlainTextEdit::keyPressEvent(e);

    const bool ctr10rShift = e->modifiers().testFlag(Qt::ControlModifier) || e->modifiers().testFlag(Qt::ShiftModifier);
    if(!c || (ctr10rShift && e->text().isEmpty())) return;

    const QString eow("~!@#$%&^()_+{}|:[].,-=?'\\");
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctr10rShift;
    const QString completionPrefix = textUnderCursor();

    if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1 || eow.contains(e->text().right(1)))){
        c->popup()->hide();
        return;
    }

    //予測変換で参照する文字列(接頭辞)とポップアップの所恋インデックスの設定
    if(completionPrefix != c->completionPrefix()){
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }

    //ポップアップウィンドウのサイズ設定
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());

    //ポップアップウィンドウの表示
    c->complete(cr);
}

void ConsoleWidget::focusInEvent(QFocusEvent *e)
{
    if(c) c->setWidget(this);

    QPlainTextEdit::focusInEvent(e);
}

void ConsoleWidget::insertCompletion(const QString &completion)
{
    if(c->widget() != this) return;

    QTextCursor tc = textCursor();

    const int extra = completion.length() - c->completionPrefix().length();

    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));

    setTextCursor(tc);
}

QString ConsoleWidget::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}
