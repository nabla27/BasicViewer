#include "recmdline.h"

ReCmdLine::ReCmdLine(QWidget *parent)
    : QLineEdit(parent)
{
    {//レイアウト色の設定
        QPalette palette = this->palette();
        palette.setColor(QPalette::Base, Qt::GlobalColor::white);
        palette.setColor(QPalette::Text, Qt::GlobalColor::black);
        setPalette(palette);
    }
    {//completerの初期設定
        setCompleter(new QCompleter());
    }
}

ReCmdLine::~ReCmdLine()
{
    delete c;
}

void ReCmdLine::setCompleter(QCompleter *completer)
{
    if(c)
        c->disconnect(this);

    c = completer;

    if(!c) return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);

    QObject::connect(c, QOverload<const QString&>::of(&QCompleter::activated),
                     this, &ReCmdLine::insertCompletion);
}


void ReCmdLine::insertCompletion(const QString &completion)
{
    if(c->widget() != this)
        return;

    int extra = completion.length() - c->completionPrefix().length();

    this->setCursorPosition(this->cursorPosition());
    this->insert(completion.right(extra));
}

QString ReCmdLine::textUnderCursor() const
{
    return this->text().split(' ').constFirst();
}

void ReCmdLine::focusInEvent(QFocusEvent *e)
{
    if(c)
        c->setWidget(this);

    QLineEdit::focusInEvent(e);
}

void ReCmdLine::keyPressEvent(QKeyEvent *e)
{
    if(c && c->popup()->isVisible())
    {
        switch(e->key()){
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

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) &&
                             e->key() == Qt::Key_E);
    if(!c || !isShortcut)
        QLineEdit::keyPressEvent(e);

    /**/

    /**/

    const bool ctr10rShift = (e->modifiers().testFlag(Qt::ControlModifier) ||
                              e->modifiers().testFlag(Qt::ShiftModifier));

    static QString eow("~!@#$%{}|:\"<>?,./;\\");
    const bool hasModifier = ((e->modifiers() != Qt::NoModifier) &&
                              !ctr10rShift);
    const QString completionPrefix = textUnderCursor();

    if(!isShortcut && (hasModifier ||
                       e->text().isEmpty() ||
                       eow.contains(e->text().right(1)))){
        c->popup()->hide();
        return;
    }

    if(completionPrefix != c->completionPrefix()){
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(100);
    c->complete(cr);
}



















QList<QString> ReparseCmdLine(const QString& cmd)
{
    bool inBracket = false;
    bool inSingleQuo = false;
    bool inDoubleQuo = false;
    QList<QString> cmdList(1);
    qsizetype index = 0;

    for(const QChar& c : cmd)
    {
        if(c == ' '){
            if(cmdList.at(index).isEmpty())
                continue;
            else if(!inBracket && !inSingleQuo && !inDoubleQuo){
                index++;
                cmdList.append("");
                continue;
            }
            else if(inBracket)
                continue;
            else
                cmdList[index] += c;
        }
        else if(c == '('){
            inBracket = true;
            cmdList[index] += c;
        }
        else if(c == ')'){
            inBracket = false;
            cmdList[index] += c;
        }
        else if(c == '\'')
            inSingleQuo = !inSingleQuo;
        else if(c == '"')
            inDoubleQuo = !inDoubleQuo;
        else
            cmdList[index] += c;
    }

    if(cmdList.last().isEmpty())
        cmdList.pop_back();

    return cmdList;
}
