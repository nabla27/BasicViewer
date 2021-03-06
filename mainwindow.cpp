#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "io/iofile.h"
#include "subwindow/imgdisplay.h"

#include "subwindow/graph2dseries.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    {//各レイアウト設定
        /* メインウィンドウ */
        QPalette paletteCenter = ui->centralwidget->palette();
        paletteCenter.setColor(QPalette::Window, Qt::GlobalColor::black);
        ui->centralwidget->setPalette(paletteCenter);
        ui->centralwidget->setAutoFillBackground(true);
        /* タブ */
        QPalette paletteTab = ui->tab_editor->palette();
        paletteTab.setColor(QPalette::Window, Qt::GlobalColor::darkGray);
        ui->tab_editor->setPalette(paletteTab);
        ui->tab_table->setPalette(paletteTab);
        ui->tab_output->setPalette(paletteTab);
        ui->tab_setting->setPalette(paletteTab);
        ui->tab_help->setPalette(paletteTab);
        ui->tab_editor->setAutoFillBackground(true);
        ui->tab_table->setAutoFillBackground(true);
        ui->tab_output->setAutoFillBackground(true);
        ui->tab_setting->setAutoFillBackground(true);
        ui->tab_help->setAutoFillBackground(true);
    }
    {
        /* textEditのタブ幅変更 */
        connect(ui->lineEdit_setE_ts, &QLineEdit::textChanged,
                ui->textEdit_editor, &TextEdit::changeTabSpace);
        /* textEditの文字折り返し設定変更 */
        connect(ui->checkBox_setE_wp, &QCheckBox::stateChanged,
                ui->textEdit_editor, &TextEdit::changeWrapMode);
        /* tableWidgetの行追加 */
        connect(ui->pushButton_sheet_rowp, &QPushButton::released,
                ui->tableWidget_table, &TableWidget::appendLineRow);
        /* tableWidgetの行削除 */
        connect(ui->pushButton_sheet_rowm, &QPushButton::released,
                ui->tableWidget_table, &TableWidget::removeLineRow);
        /* tableWidgetの列追加 */
        connect(ui->pushButton_sheet_colp, &QPushButton::released,
                ui->tableWidget_table, &TableWidget::appendLineCol);
        /* tableWidgetの列削除 */
        connect(ui->pushButton_sheet_colm, &QPushButton::released,
                ui->tableWidget_table, &TableWidget::removeLineCol);
        /* tableWidgetのクリア */
        connect(ui->pushButton_sheet_clear, &QPushButton::released,
                ui->tableWidget_table, &TableWidget::clear);
        /* tableWidgetのファイル名設定 */
        connect(ui->lineEdit_name_sheet, &QLineEdit::textChanged,
                ui->tableWidget_table, &TableWidget::setSheetName);
        /* gnuplotの実行パス設定変更 */
        connect(ui->lineEdit_setG_p, &QLineEdit::textChanged, [this](){
            gnuplot->setExePath(ui->lineEdit_setG_p->text());
        });
        /* gnuplotの事前コマンド設定変更 */
        connect(ui->textEdit_setG_precmd, &TextEdit::textChanged, [this](){
            gnuplot->setPreCmd(ui->textEdit_setG_precmd->toPlainText());
        });
        /* gnuplotのオプションコマンド設定変更 */
        connect(ui->lineEdit_setG_sheetplotop, &QLineEdit::textChanged, [this](){
            gnuplot->setOptionCmd(ui->lineEdit_setG_sheetplotop->text());
        });
    }
    {//gnuplotに関する初期設定
        gnuplot = new Gnuplot();
        gnuplot->setOutBrowser(ui->textBrowser_output);
        ui->tableWidget_table->setGnuplot(gnuplot);
    }
    {//設定を読み込む
        loadXmlSetting();
    }
}

MainWindow::~MainWindow()
{
    saveXmlSetting();
    delete ui;
}

/* 文字サイズ変更 */
void MainWindow::on_spinBox_setE_fs_valueChanged(int arg1)
{
    QFont font = ui->textEdit_editor->font();
    font.setPointSize(arg1);
    ui->textEdit_editor->setFont(font);
    ui->lineEdit_setE_test->setFont(font);
    ui->lineEdit_cmdline->setFont(font);
}
/* フォントの変更 */
void MainWindow::on_fontComboBox_setE_f_textHighlighted(const QString &arg1)
{
    QFont font = ui->textEdit_editor->font();
    font.setFamily(arg1);
    ui->textEdit_editor->setFont(font);
    ui->lineEdit_setE_test->setFont(font);
    ui->lineEdit_cmdline->setFont(font);
}
/* 斜体設定の変更 */
void MainWindow::on_checkBox_setE_ir_stateChanged(int arg1)
{
    QFont font = ui->textEdit_editor->font();
    font.setItalic(arg1);
    ui->textEdit_editor->setFont(font);
    ui->lineEdit_setE_test->setFont(font);
}
/* ボールド設定の変更 */
void MainWindow::on_checkBox_setE_bl_stateChanged(int arg1)
{
    QFont font = ui->textEdit_editor->font();
    font.setBold(arg1);
    ui->textEdit_editor->setFont(font);
    ui->lineEdit_setE_test->setFont(font);
}

/* fileTreeのダブルクリックでEditorやSheetなどを変更 */
void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item->parent() == nullptr) return;

    const QString parentTitle = item->parent()->text(column);

    if(parentTitle == "Script")
    {
        ui->tabWidget->setCurrentIndex(0);                                        //tabページをeditor(0ページ目)へ変更
        if(ui->treeWidget->scriptIndex() >= 0) {
            const QString beforeName = BasicSet::tmpDirectory + ui->treeWidget->currentScriptName();
            (void)toFileTxt(beforeName, ui->textEdit_editor->toPlainText());
        }
        ui->treeWidget->indexChange(item, column);
        const QString afterName = BasicSet::tmpDirectory + ui->treeWidget->currentScriptName();
        ui->lineEdit_name_script->setText(ui->treeWidget->currentScriptName());
        ui->textEdit_editor->setPlainText(readFileTxt(afterName));
    }
    else if(parentTitle == "Sheet")
    {
        ui->tabWidget->setCurrentIndex(1);                                        //tabページをsheet(1ページ目)へ移動
        if(ui->treeWidget->sheetIndex() >= 0){
            const QString beforeName = BasicSet::tmpDirectory + ui->treeWidget->currentSheetName();
            (void)toFileCsv(beforeName, ui->tableWidget_table->getData<QString>());
        }
        ui->treeWidget->indexChange(item, column);
        const QString afterName = BasicSet::tmpDirectory + ui->treeWidget->currentSheetName();
        ui->lineEdit_name_sheet->setText(ui->treeWidget->currentSheetName());
        ui->tableWidget_table->setData(readFileCsv(afterName));
    }
    else if(parentTitle == "Other")
    {
        ui->treeWidget->indexChange(item, column);                                //ダブルクリックによるfileTreeのインデックス変更
        const QString fileName = ui->treeWidget->currentOtherName();              //ダブルクリックされたファイル名
        const qsizetype extStartIndex = fileName.lastIndexOf('.');                //ドットのインデックス

        if(extStartIndex != qsizetype(-1) && fileName.size() > extStartIndex + 1) //拡張子を含む(ドット以降に文字がある)
        {
            const QString extension = fileName.sliced(extStartIndex + 1);         //拡張子

            /* 画像の表示 */
            if(ImageDisplay::isValidExtension(extension)){
                ImageDisplay *imageDisp = new ImageDisplay();
                imageDisp->setImageFile(BasicSet::tmpDirectory + fileName);
                imageDisp->show();
            }
        }
    }
}

/* 実行ボタン */
void MainWindow::on_pushButton_excute_released()
{
    /* scriptが適切に選択されていなければ無効 */
    if(ui->treeWidget->scriptIndex() < 0){
        ui->textBrowser_output->outMessage("script is not chosen.", "handling");
        return;
    }

    /* textEditorのエラー行番号リセット */
    ui->textEdit_editor->setErrorLineNumber(-1);

    /* ファイルの上書き保存 */
    (void)toFileTxt(BasicSet::tmpDirectory + ui->treeWidget->currentScriptName(),
                    ui->textEdit_editor->toPlainText());
    (void)toFileCsv(BasicSet::tmpDirectory + ui->treeWidget->currentSheetName(),
                    ui->tableWidget_table->getData<QString>());

    /* 実行するコマンドの生成 */
    QList<QString> cmdList;
    cmdList << "load '" + ui->treeWidget->currentScriptName() + "'\n";

    /* gnuplotの実行 */
    gnuplot->exc(ui->treeWidget->currentScriptProcess(), cmdList);

    /* エラーがあれば、そのエラー行を設定する */
    connect(ui->treeWidget->currentScriptProcess(), &QProcess::readyReadStandardError, [this](){
         const int errorLineNumber = gnuplot->getErrorLineNumber();
         ui->textEdit_editor->setErrorLineNumber(errorLineNumber);
         ui->textEdit_editor->highlightLine();
    });

    /* 再ハイライト */
    ui->textEdit_editor->highlightLine();
}

/* cmdLineの実行 */
void MainWindow::on_lineEdit_cmdline_returnPressed()
{
    const QList<QString> cmdList =  parseCmdLine(ui->lineEdit_cmdline->text());

    if(cmdList.size() < 1) { return; }       //何も入力されず、Enterが押された場合は無効

    const QString firstCmd = cmdList.at(0);  //先頭コマンド

    if(firstCmd == "gnuplot")
    {
        /* もしscriptが選択されていなければ(=nullptr)、textBrowserにエラーメッセージを送る */
        if(ui->treeWidget->currentScriptProcess() == nullptr){
            ui->textBrowser_output->outMessage("script is not chosen.", "handling");
            return;
        }

        if(cmdList.size() > 1)
        {
            const QString secondCmd = cmdList.at(1);
            if(secondCmd == "close") { ui->treeWidget->currentScriptProcess()->close(); return; }
            else if(secondCmd == "kill") { ui->treeWidget->currentScriptProcess()->kill(); return; }
            else
            {
                QList<QString> plotCmd;  //gnuplotに実行させるコマンド列
                QString cmd;             //stack

                /* ; ごとにコマンドを区切る */
                for(qsizetype i = 1; i < cmdList.size(); ++i){
                    if(cmdList.at(i) == ";") { plotCmd << cmd + "\n"; cmd.clear(); }
                    else { cmd += cmdList.at(i) + " "; }
                }
                if(!cmd.isEmpty()) { plotCmd << cmd + "\n"; }

                /* 実行 */
                gnuplot->exc(ui->treeWidget->currentScriptProcess(), plotCmd);
            }
        }
    }
    else if(firstCmd == "Debug")
    {
        Graph2DSeries *graph = new Graph2DSeries(ui->tableWidget_table);
        graph->show();
    }
}





































