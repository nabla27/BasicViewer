#ifndef LAYOUTPARTS_H
#define LAYOUTPARTS_H
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QToolBox>
#include <QLabel>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QComboBox>
#include <QSpacerItem>
#include <QSpinBox>
#include <QPushButton>

#define SETTING_EDIT_LWIDTH 110
#define SETTING_EDIT_SWIDTH 35
#define QT_GLOBAL_COLOR_COUNT 19
#define SETTING_LABEL_WIDTH 80





class RGBEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    RGBEditLayout(QWidget *parent);

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setEditMaximumWidth(const int width) { rEdit->setMaximumWidth(width);
                                                gEdit->setMaximumWidth(width);
                                                bEdit->setMaximumWidth(width); }

public slots:
    void setColor(const QColor& color);
    void setColor(int eNum);
    void setVisible(bool visible);
    void setReadOnly(bool readOnly);

signals:
    void colorEdited(const QColor& color);

private:
    QColor getColor() { return QColor(rEdit->text().toInt(), gEdit->text().toInt(), bEdit->text().toInt()); }

private:
    QLabel *label;
    QLineEdit *rEdit;
    QLineEdit *gEdit;
    QLineEdit *bEdit;
    QSpacerItem *spacer;
};








class ComboEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    ComboEditLayout(QWidget *parent, const QString& text = "");

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setComboMaximumWidth(const int width) { combo->setMaximumWidth(width); }
    void insertComboItems(int index, const QStringList& texts) { combo->insertItems(index, texts); }
    int currentComboIndex() const { return combo->currentIndex(); }
    QString currentComboText() const { return combo->currentText(); }

public slots:
    void setComboCurrentIndex(const int index) { combo->setCurrentIndex(index); }
    void setVisible(bool visible);

signals:
    void currentComboIndexChanged(int index);

private:
    QLabel *label;
    QComboBox *combo;
    QSpacerItem *spacer;
};












class LineEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    LineEditLayout(QWidget *parent, const QString& text = "");

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setLineEditMaximumWidth(const int width) { lineEdit->setMaximumWidth(width); }
    void setReadOnly(const bool flag) { lineEdit->setReadOnly(flag); }
    QString lineEditText() const { return lineEdit->text(); }

public slots:
    void setLineEditText(const QString& text) { lineEdit->setText(text); }
    void setVisible(bool visible);

signals:
    void lineTextEdited(const QString& text);

private:
    QLabel *label;
    QLineEdit *lineEdit;
    QSpacerItem *spacer;
};









class SpinBoxEditLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    SpinBoxEditLayout(QWidget *parent, const QString& text = "");

public:
    void setLabelMinimumWidth(const int width) { label->setMinimumWidth(width); }
    void setSpinBoxMaximumWidth(const int width) { spinBox->setMaximumWidth(width); }
    int spinBoxValue() const { return spinBox->value(); }

public slots:
    void setSpinBoxValue(const int value) { spinBox->setValue(value); }
    void setVisible(bool visible);

signals:
    void spinBoxValueChanged(const int value);

private:
    QLabel *label;
    QSpinBox *spinBox;
    QSpacerItem *spacer;
};












class PushButtonLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    PushButtonLayout(QWidget *parent, const QString& text = ""){
        button = new QPushButton(text, parent);
        addWidget(button, 0, Qt::AlignCenter);
        connect(button, &QPushButton::released, [this](){ emit buttonReleased(); });
    };

public:
    void setButtonMaximumWidth(const int width) { button->setMaximumWidth(width); }
    void setButtonMinimumWidth(const int width) { button->setMinimumWidth(width); }

private:
    QPushButton *button;

signals:
    void buttonReleased();
};









class BlankSpaceLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    BlankSpaceLayout(const int width, const int height){
        spacer = new QSpacerItem(width, height, QSizePolicy::Maximum, QSizePolicy::Maximum);
        addItem(spacer);
    }

private:
    QSpacerItem *spacer;
};















#endif // LAYOUTPARTS_H
