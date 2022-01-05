#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>


void MainWindow::loadXmlSetting()
{
    using namespace boost::property_tree;

    if(!QFile::exists(BasicSet::settingFile)) return;

    ptree pt;
    read_xml(BasicSet::settingFile.toUtf8().constData(), pt);

    if(boost::optional<int> value = pt.get_optional<int>("root.TextEditor.textPointSize")){
        ui->spinBox_setE_fs->setValue(value.value());
    }
    if(boost::optional<int> value = pt.get_optional<int>("root.TextEditor.font")){
        ui->fontComboBox_setE_f->setCurrentIndex(value.value());
    }
    if(boost::optional<std::string> text = pt.get_optional<std::string>("root.TextEditor.tabSpace")){
        ui->lineEdit_setE_ts->setText(QString::fromStdString(text.value()));
    }
    if(boost::optional<bool> value = pt.get_optional<bool>("root.TextEditor.isWrap")){
        ui->checkBox_setE_wp->setChecked(value.value());
    }
    if(boost::optional<bool> value = pt.get_optional<bool>("root.TextEditor.isItaric")){
        ui->checkBox_setE_ir->setChecked(value.value());
    }
    if(boost::optional<bool> value = pt.get_optional<bool>("root.TextEditor.isBold")){
        ui->checkBox_setE_bl->setChecked(value.value());
    }

    if(boost::optional<std::string> text = pt.get_optional<std::string>("root.Gnuplot.exePath")){
        ui->lineEdit_setG_p->setText(QString::fromStdString(text.value()));
    }
    if(boost::optional<std::string> text = pt.get_optional<std::string>("root.Gnuplot.preCmd")){
        ui->textEdit_setG_precmd->setPlainText(QString::fromStdString(text.value()));
    }
    if(boost::optional<std::string> text = pt.get_optional<std::string>("root.Gnuplot.plotOption")){
        ui->lineEdit_setG_sheetplotop->setText(QString::fromStdString(text.value()));
    }
}



void MainWindow::saveXmlSetting()
{
    using namespace boost::property_tree;

    ptree pt;

    ptree& textEditor = pt.add("root.TextEditor", "");

    textEditor.add("textPointSize", ui->spinBox_setE_fs->value());
    textEditor.add("font", ui->fontComboBox_setE_f->currentIndex());
    textEditor.add("tabSpace", ui->lineEdit_setE_ts->text().toUtf8().constData());
    textEditor.add("isWrap", (int)ui->checkBox_setE_wp->isChecked());
    textEditor.add("isItaric", (int)ui->checkBox_setE_ir->isChecked());
    textEditor.add("isBold", (int)ui->checkBox_setE_bl->isChecked());

    ptree& gnuplot = pt.add("root.Gnuplot", "");

    gnuplot.add("exePath", ui->lineEdit_setG_p->text().toUtf8().constData());
    gnuplot.add("preCmd", ui->textEdit_setG_precmd->toPlainText().toUtf8().constData());
    gnuplot.add("plotOption", ui->lineEdit_setG_sheetplotop->text().toUtf8().constData());

    const int indent = 4;
    write_xml(BasicSet::settingFile.toUtf8().constData(), pt, std::locale(),
              xml_writer_make_settings<std::string>(' ', indent, "utf-8"));
}
