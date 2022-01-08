#include "imgdisplay.h"


PaintImage::PaintImage(QWidget *parent)
    : QGraphicsView(parent)
{

}

PaintImage::~PaintImage()
{

}


void PaintImage::paintEvent(QPaintEvent*)
{
    QPainter widgetPainter(viewport());
    QImage qimg = img.scaled(viewport()->width(),
                             viewport()->height(),
                             Qt::KeepAspectRatio,
                             Qt::FastTransformation);

    widgetPainter.drawImage(0, 0, qimg);
}











ImageDisplay::ImageDisplay(QWidget *parent)
    : QWidget(parent)
{
    image = new PaintImage(this);

    hLayout = new QHBoxLayout(this);
    hLayout->addWidget(image, 0);

    setAttribute(Qt::WA_DeleteOnClose);
}

ImageDisplay::~ImageDisplay()
{
    delete image;
    delete hLayout;
}

bool ImageDisplay::setImageFile(const QString& fullpath)
{
    QImage img(fullpath);          //画像ファイルのパスfullpathを渡して画像を読み取る
    image->setImage(img);          //画像をセット

    return !img.isNull();          //画像が読み取れたらtrue,失敗したらfalseを返す
}

bool ImageDisplay::isValidExtension(const QString &ext)
{
    if(ext.compare("BMP", Qt::CaseInsensitive) != 0 &&
       ext.compare("GIF", Qt::CaseInsensitive) != 0 &&
       ext.compare("JPG", Qt::CaseInsensitive) != 0 &&
       ext.compare("JPEG", Qt::CaseInsensitive) != 0 &&
       ext.compare("PNG", Qt::CaseInsensitive) != 0 &&
       ext.compare("PBM", Qt::CaseInsensitive) != 0 &&
       ext.compare("PGM", Qt::CaseInsensitive) != 0 &&
       ext.compare("PPM", Qt::CaseInsensitive) != 0 &&
       ext.compare("XBM", Qt::CaseInsensitive) != 0 &&
       ext.compare("XPM", Qt::CaseInsensitive) != 0)
    {
        return false;
    }

    return true;
}














