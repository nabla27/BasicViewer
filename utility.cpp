#include "utility.h"



const QStringList colorNameList()
{
    QStringList colorList;

    colorList = enumToStrings(Qt::GlobalColor(0));
    colorList << "custom";

    return colorList;
}

const QStringList imgFormatList()
{
    QStringList formatList;

    for(const QByteArray& byte : QImageWriter::supportedImageFormats()){
        formatList << byte.constData();
    }

    return formatList;
}
