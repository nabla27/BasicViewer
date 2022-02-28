#ifndef EDITORSETTINGWIDGET_H
#define EDITORSETTINGWIDGET_H

#include <QWidget>

class EditorSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditorSettingWidget(QWidget *parent = nullptr);

private:
    void initializeLayout();

signals:

};

#endif // EDITORSETTINGWIDGET_H
