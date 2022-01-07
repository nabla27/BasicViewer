#ifndef GRAPHIC3DSCATTER_H
#define GRAPHIC3DSCATTER_H
#include <QWidget>
#include <Q3DScatter>
#include <QHBoxLayout>

class Graphic3DScatter : public QWidget
{
    Q_OBJECT
public:
    explicit Graphic3DScatter(QWidget *parent = nullptr);
    ~Graphic3DScatter();

public:
    void setData(const QList<QList<QList<float> > >& list);

private:
    Q3DScatter *graph;
    QWidget *container;
    QHBoxLayout *hLayout;
    QList<QScatter3DSeries*> series;
    QValue3DAxis *xAxis;
    QValue3DAxis *yAxis;
    QValue3DAxis *zAxis;
};

#endif // GRAPHIC3DSCATTER_H
