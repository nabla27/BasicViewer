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
















