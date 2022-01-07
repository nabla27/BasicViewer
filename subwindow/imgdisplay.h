#ifndef IMGDISPLAY_H
#define IMGDISPLAY_H
#include <QWidget>
#include <QGraphicsView>
#include <QImage>
#include <QPainter>
#include <QHBoxLayout>
#include <QPaintEvent>

class PaintImage : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PaintImage(QWidget *panrent = nullptr);
    ~PaintImage();

public:
    void setImage(const QImage& img) { this->img = img; viewport()->update(); }

private:
    void paintEvent(QPaintEvent *event);

private:
    QImage img;
};






class ImageDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplay(QWidget* parent = nullptr);
    ~ImageDisplay();

public:
    bool setImageFile(const QString& fullpath);

private:
    PaintImage *image;
    QHBoxLayout *hLayout;
};









#endif // IMGDISPLAY_H
