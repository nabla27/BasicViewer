#ifndef GRAPHIC3DBAR_H
#define GRAPHIC3DBAR_H
#include <QWidget>
#include <Q3DBars>
#include <QHBoxLayout>

class Graphic3DBar : public QWidget
{
    Q_OBJECT
public:
    explicit Graphic3DBar(QWidget *parent = nullptr);
    ~Graphic3DBar();

public:
    void setData(const QList<QList<QList<float> > >& list);

private:
    Q3DBars *graph;
    QWidget *container;
    QHBoxLayout *hLayout;
    QList<QBar3DSeries*> series;
    QCategory3DAxis *rowAxis;
    QCategory3DAxis *colAxis;
};

#endif // GRAPHIC3DBAR_H
