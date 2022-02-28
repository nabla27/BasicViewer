#ifndef EDITORSETTINGWIDGET_H
#define EDITORSETTINGWIDGET_H

#include <QWidget>
#include "subwindow/layoutparts.h"

class EditorSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditorSettingWidget(QWidget *parent = nullptr);

private:
    void initializeLayout();

private:
    void setBackgroundColorComboEditable(const int index);
    void setTextColorComboEditable(const int index);
    void setHighlightColorComboEditable(const int index);
    void setCommentColorComboEditable(const int index);

private:
    ComboEditLayout *backgroundColorCombo;
    RGBEditLayout *backgroundColorRgb;
    ComboEditLayout *textColorCombo;
    RGBEditLayout *textColorRgb;
    SpinBoxEditLayout *textSizeSpin;
    ComboEditLayout *fontCombo;
    LineEditLayout *tabSpaceEdit;
    CheckBoxLayout *checkWrap;
    CheckBoxLayout *checkItaric;
    CheckBoxLayout *checkBold;
    ComboEditLayout *highlightColorCombo;
    RGBEditLayout *highlightColorRgb;
    ComboEditLayout *commentColorCombo;
    RGBEditLayout *commentColorRgb;

signals:
    void backgroundColorSet(const QColor& color);
    void textColorSet(const QColor& color);
    void textSizeSet(const int ps);
    void textFontSet(const int index);
    void tabSpaceSet(const double& space);
    void checkWrapSet(const bool wrap);
    void checkItaricSet(const bool iraric);
    void checkBoldSet(const bool bold);
    void highlightColorSet(const QColor& color);
    void commentColorSet(const QColor& color);
};

#endif // EDITORSETTINGWIDGET_H
