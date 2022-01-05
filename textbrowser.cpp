#include "textbrowser.h"

TextBrowser::TextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{

}

void TextBrowser::outMessage(const QString &message, const QString &type)
{
    /* 以前受け取ったメッセージのタイプと異なれば、タイプタイトルを表記する */
    if(type != this->messageType && message != ""){
        this->messageType = type;
        this->insertPlainText("[" + type + "]\n");
    }

    /* 改行文字で受け取ったメッセージを分割 */
    const QStringList messageList = message.split('\n');

    /* 分割されたメッセージを空行を無視して出力 */
    for(const QString& str : messageList)
    {
        if(str != "\n" &&
           str != "\r" &&
           str != "\r\n" &&
           str != "")
            this->insertPlainText(str + "\n");
    }
}
