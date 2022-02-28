#include "editorsettingwidget.h"

EditorSettingWidget::EditorSettingWidget(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_DeleteOnClose);

    setMinimumSize(240, 400);
    setMaximumSize(240, 400);

    initializeLayout();
}

void EditorSettingWidget::initializeLayout()
{
    if(!layout()) delete layout();

    QVBoxLayout *layout = new QVBoxLayout(this);
    backgroundColorCombo = new ComboEditLayout(this, "Background color");
    backgroundColorRgb = new RGBEditLayout(this);
    textColorCombo = new ComboEditLayout(this, "Text color");
    textColorRgb = new RGBEditLayout(this);
    textSizeSpin = new SpinBoxEditLayout(this, "Text size");
    fontCombo = new ComboEditLayout(this, "Font");
    tabSpaceEdit = new LineEditLayout(this, "Tab spacing");
    checkWrap = new CheckBoxLayout(this, "Wrap line");
    checkItaric = new CheckBoxLayout(this, "Itaric text");
    checkBold = new CheckBoxLayout(this ,"Bold text");
    highlightColorCombo = new ComboEditLayout(this, "Highlight color");
    highlightColorRgb = new RGBEditLayout(this);
    commentColorCombo = new ComboEditLayout(this, "Comment color");
    commentColorRgb = new RGBEditLayout(this);
    QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    setLayout(layout);
    layout->addLayout(backgroundColorCombo);
    layout->addLayout(backgroundColorRgb);
    layout->addLayout(textColorCombo);
    layout->addLayout(textColorRgb);
    layout->addLayout(textSizeSpin);
    layout->addLayout(fontCombo);
    layout->addLayout(tabSpaceEdit);
    layout->addLayout(checkWrap);
    layout->addLayout(checkItaric);
    layout->addLayout(checkBold);
    layout->addLayout(highlightColorCombo);
    layout->addLayout(highlightColorRgb);
    layout->addLayout(commentColorCombo);
    layout->addLayout(commentColorRgb);
    layout->addItem(spacer);

    const int labelWidth = 100;
    backgroundColorCombo->setLabelMinimumWidth(labelWidth);
    backgroundColorRgb->setLabelMinimumWidth(labelWidth);
    textColorCombo->setLabelMinimumWidth(labelWidth);
    textColorRgb->setLabelMinimumWidth(labelWidth);
    textSizeSpin->setLabelMinimumWidth(labelWidth);
    fontCombo->setLabelMinimumWidth(labelWidth);
    tabSpaceEdit->setLabelMinimumWidth(labelWidth);
    checkWrap->setLabelMinimumWidth(labelWidth);
    checkItaric->setLabelMinimumWidth(labelWidth);
    checkBold->setLabelMinimumWidth(labelWidth);
    highlightColorCombo->setLabelMinimumWidth(labelWidth);
    highlightColorRgb->setLabelMinimumWidth(labelWidth);
    commentColorCombo->setLabelMinimumWidth(labelWidth);
    commentColorRgb->setLabelMinimumWidth(labelWidth);

    connect(backgroundColorCombo, &ComboEditLayout::currentComboIndexChanged, this, &EditorSettingWidget::setBackgroundColorComboEditable);
    connect(backgroundColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::backgroundColorSet);
    connect(textColorCombo, &ComboEditLayout::currentComboIndexChanged, this, &EditorSettingWidget::setTextColorComboEditable);
    connect(textColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::textColorSet);
    connect(textSizeSpin, &SpinBoxEditLayout::spinBoxValueChanged, this, &EditorSettingWidget::textSizeSet);
    connect(fontCombo, &ComboEditLayout::currentComboIndexChanged, this, &EditorSettingWidget::textFontSet);
    connect(tabSpaceEdit, &LineEditLayout::lineValueEdited, this, &EditorSettingWidget::tabSpaceSet);
    connect(checkWrap, &CheckBoxLayout::checkBoxToggled, this, &EditorSettingWidget::checkWrapSet);
    connect(checkItaric, &CheckBoxLayout::checkBoxToggled, this, &EditorSettingWidget::checkItaricSet);
    connect(checkBold, &CheckBoxLayout::checkBoxToggled, this, &EditorSettingWidget::checkBoldSet);
    connect(highlightColorCombo, &ComboEditLayout::currentComboIndexChanged, this, &EditorSettingWidget::setHighlightColorComboEditable);
    connect(highlightColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::highlightColorSet);
    connect(commentColorCombo, &ComboEditLayout::currentComboIndexChanged, this, &EditorSettingWidget::setCommentColorComboEditable);
    connect(commentColorRgb, &RGBEditLayout::colorEdited, this, &EditorSettingWidget::commentColorSet);
}

void EditorSettingWidget::setBackgroundColorComboEditable(const int index)
{

}

void EditorSettingWidget::setTextColorComboEditable(const int index)
{

}

void EditorSettingWidget::setHighlightColorComboEditable(const int index)
{

}

void EditorSettingWidget::setCommentColorComboEditable(const int index)
{

}


