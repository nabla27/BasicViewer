#include "filetree.h"

FileTree::FileTree(QWidget *parent)
    : QTreeWidget(parent)
{
    {//シグナル&スロット
        /* contextMenuの表示 */
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        this->normalMenu = new QMenu(this);
        connect(this, &FileTree::customContextMenuRequested,
                this, &FileTree::onCustomContextMenu);
        /* ディレクトリの変更を監視。変更があれば、TreeとListに反映させる */
        watchDir = new QFileSystemWatcher(QStringList() << BasicSet::tmpDirectory);
        connect(watchDir, &QFileSystemWatcher::directoryChanged,
                this, &FileTree::updateFileTree);
    }
    {//contextMenuのメニュー
        /* add */
        QAction *actAdd = new QAction("add", normalMenu);
        normalMenu->addAction(actAdd);
        connect(actAdd, &QAction::triggered, this, &FileTree::addFile);
        /* new */
        QAction *actNew = new QAction("new", normalMenu);
        normalMenu->addAction(actNew);
        connect(actNew, &QAction::triggered, this, &FileTree::newFile);
        /* rename */
        QAction *actRename = new QAction("rename", normalMenu);
        normalMenu->addAction(actRename);
        connect(actRename, &QAction::triggered, this, &FileTree::renameFile);
        /* remove */
        QAction *actRemove = new QAction("remove", normalMenu);
        normalMenu->addAction(actRemove);
        connect(actRemove, &QAction::triggered, this, &FileTree::removeFile);
        /* save */
        QAction *actSave = new QAction("save", normalMenu);
        normalMenu->addAction(actSave);
        connect(actSave, &QAction::triggered, this, &FileTree::saveFile);
    }
    QDir dir(BasicSet::tmpDirectory);
    if(!dir.exists()){
        QDir currentDir("./");
        currentDir.mkdir(BasicSet::tmpDirectory);
    }
    reloadFileTree();
}

FileTree::~FileTree()
{
    foreach(QProcess *const process, scriptList){
        process->close();
        delete process;
    }
    delete normalMenu; //scriptTree,sheetTree,otherTreeもdeleteされる
    delete watchDir;
}


QStringList FileTree::filenameList(const QString str) const
{
    QStringList filenameList;
    for(qsizetype i = 0, inf = scriptTree->childCount(); i < inf; ++i)
        filenameList << str + scriptTree->child(i)->text(0) + str;
    for(qsizetype i = 0, inf = sheetTree->childCount(); i < inf; ++i)
        filenameList << str + sheetTree->child(i)->text(0) + str;
    for(qsizetype i = 0, inf = otherTree->childCount(); i < inf; ++i)
        filenameList << str + otherTree->child(i)->text(0) + str;

    return filenameList;
}

//#####################
// FileTreeの更新
//#####################

/* fileTreeを一度リセットし、リストを更新する。プロセスも新しくなる。 */
void FileTree::reloadFileTree()
{
    /* treeWidgetをクリア */
    this->clear();

    /* ヘッダーを非表示 */
    this->setHeaderHidden(true);

    /* scriptフォルダーの作成 */
    scriptTree = new QTreeWidgetItem(this);
    scriptTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    scriptTree->setText(0, "Script");
    /* sheetフォルダーの作成 */
    sheetTree = new QTreeWidgetItem(this);
    sheetTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    sheetTree->setText(0, "Sheet");
    /* otherフォルダーの作成 */
    otherTree = new QTreeWidgetItem(this);
    otherTree->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    otherTree->setText(0, "Other");

    /* プロセスを閉じてdelete */
    foreach(QProcess *const process, scriptList){
        process->close();
        delete process;
    }

    /* リストの初期化 */
    scriptList.clear();
    sheetList.clear();
    otherList.clear();

    /* フォルダーにファイルを追加 */
    updateFileTree();
}

/* ディレクトリからファイルを追加 */
void FileTree::updateFileTree()
{
    /* ディレクトリのファイル情報を取得 */
    QDir dir(BasicSet::tmpDirectory);
    QFileInfoList fileList = dir.entryInfoList();

    /* ファイルを順に捜査して、拡張子ごとにフォルダーに分ける。すでにtreeに登録されているファイルは無視する */
    for(const QFileInfo& fileInfo : fileList)
    {
        const QString filename = fileInfo.fileName();

        if(filename.contains(".txt")){
            if(!scriptList.contains(filename))
                addScriptFile(filename);
        }
        else if(filename.contains(".csv")){
            if(!sheetList.contains(filename))
                addSheetFile(filename);
        }
        else if(filename == "." || filename == "..")
            continue;
        else if(!otherList.contains(filename))
            addOtherFile(filename);
        else
            continue;
    }
}


//#########################
// 各ファイルの追加
//#########################
/* scriptの追加 */
void FileTree::addScriptFile(const QString &filename)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(scriptTree);
    item->setText(0, filename);              //アイテムの追加
    QProcess *process = new QProcess(this);  //scriptファイル一つにプロセス一つを持たせる
    scriptList.insert(filename, process);    //リストへ追加
}
/* sheetの追加 */
void FileTree::addSheetFile(const QString &filename)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(sheetTree);
    item->setText(0, filename);     //アイテムの追加
    sheetList.insert(filename);     //リストへ追加
}
/* otherの追加 */
void FileTree::addOtherFile(const QString &filename)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(otherTree);
    item->setText(0, filename);     //アイテムの追加
    otherList.insert(filename);     //リストへ追加
}

//###################
// スロット関数
//###################
/* 右クリックによるcontextMenuの表示 */
void FileTree::onCustomContextMenu(const QPoint& point)
{
    if(this->selectedItems().count() < 1)  //ファイル名以外の部分をクリックした場合は無効
        return;
    else                                   //クリック箇所の座標をもとにcontextMenuの表示
        this->normalMenu->exec(this->viewport()->mapToGlobal(point));
}

/* 左ダブルクリックによるカレントインデックスの変更 */
void FileTree::indexChange(QTreeWidgetItem *item, int)
{
    if(item->parent() == nullptr) return;

    const QString parentTitle = item->parent()->text(0);
    const int index = currentIndex().row();

    if(parentTitle == "Script")
        currentScriptIndex = index;
    else if(parentTitle == "Sheet")
        currentSheetIndex = index;
    else if(parentTitle == "Other")
        currentOtherIndex = index;
}

/* ファイルの追加メニュー */
void FileTree::addFile()
{
    /* ファイルダイアログの表示とファイル名の取得 */
    const QStringList filePath = QFileDialog::getOpenFileNames(this);  //選択された(複数可)ファイルのフルパスを取得

    /* 選択されたファイルを一つずつ処理 */
    for(const QString& fullPath : filePath)
    {
        const QString fileName = (fullPath.split('/')).constLast();    //選択されたファイル名

        if(!filenameList().contains(fileName))                         //もし同名のファイルがなければ
        {
            const bool success = QFile::copy(fullPath, BasicSet::tmpDirectory + fileName);                 //コピーしてもってくる
            if(!success) { QMessageBox::critical(this, "Error", "could not copy the " + fileName); }   //コピー失敗時のエラーメッセージ
        }
        else                                                           //もし同名のファイルがあれば、エラーメッセージの表示
        {
            QMessageBox::critical(this,                                //parent
                                  "Error",                             //Title
                                  "Same name \"" + fileName            //message
                                  + "\" already exists!!");
        }
    }
}

/* 新規ファイルを作成 */
void FileTree::newFile()
{
    /* 追加するファイルの項目名(Script or Sheet or Other)を取得 */
    QString parentTitle;
    if(this->selectedItems().takeAt(0)->parent() == nullptr)               //項目名がクリックされた場合(=その親treeはなし)
        parentTitle = this->selectedItems().takeAt(0)->text(0);
    else                                                                   //ファイル名がクリックされた場合(=その親treeが存在)
        parentTitle = this->selectedItems().takeAt(0)->parent()->text(0);  //親treeの名前を取得

    /* 新規ファイルの名前を入力するダイアログを表示 */
    QString newFileName;
    for(;;)                                                            //有効なファイル名が入力されるまでループ
    {
        bool isok = false;
        newFileName = QInputDialog::getText(this,
                                            "FileTree",
                                            "new file name",
                                            QLineEdit::EchoMode::Normal,
                                            "",
                                            &isok);

        if(!isok || newFileName.isEmpty()) return;                     //×ボタンでnewFileキャンセル
        if(!newFileName.contains('.'))                                 //ファイル名に拡張子が含まれていない場合、エラーメッセージを表示
        {
            QMessageBox::critical(this,
                                  "Error",
                                  "extension is not included.");
        }
        else if(filenameList().contains(newFileName))                  //すでに同じ名前のファイルがあればエラーメッセージを表示
        {
            QMessageBox::critical(this,                                //parent
                                  "Error",                             //Title
                                  "Same name \"" + newFileName         //message
                                  + "\" already exists!!");
        }
        else
            break;                                                     //同じ名前がなければ決定
    }

    /* ファイルの作成 */
    QFile file(BasicSet::tmpDirectory + newFileName);
    const bool success = file.open(QIODevice::OpenModeFlag::NewOnly);  //ファイルの作成

    /* ファイルをリストへ追加 */
    if(success)
    {
        if(newFileName.contains(".txt"))
            addScriptFile(newFileName);
        else if(newFileName.contains(".csv"))
            addSheetFile(newFileName);
        else
            addOtherFile(newFileName);
    }
    else //ファイルの作成に失敗したらエラーメッセージ
    {
        QMessageBox::critical(this,                                //parent
                              "Error",                             //Title
                              "could not create file.");           //message
    }
}

/* ファイルの名前変更 */
void FileTree::renameFile()
{
    /* ファイル名でなく、項目名(Script,Sheet,Other)が押された場合は無視 */
    if(selectedItems().takeAt(0)->parent() == nullptr) return;

    /* renameするファイルの元の名前 */
    const QString oldFileName = selectedItems().takeAt(0)->text(0);

    bool isok = false;
    const QString message = "new file name";
    QString newFileName;

    /* ファイル名を入力するダイアログの表示。有効な名前が入力されるまでループ */
    for(;;)
    {
        newFileName = QInputDialog::getText(this,
                                            "rename",
                                            message,
                                            QLineEdit::EchoMode::Normal,
                                            oldFileName,
                                            &isok);

        if(!isok || newFileName.isEmpty()) return;                     //×などウィンドウが閉じられたら中断
        if(!newFileName.contains('.'))                                 //拡張子が含まれていない場合はエラー
        {
            QMessageBox::critical(this,
                                  "Error",
                                  "extension is not included.");
        }
        else if(filenameList().contains(newFileName))                  //すでに同じ名前があればエラー
        {
            QMessageBox::critical(this,                                //parent
                                  "Error",                             //Title
                                  "Same name \"" + newFileName         //message
                                  + "\" already exists!!");
        }
        else
            break;
    }

    /* ディレクトリのファイル名とfileTreeのファイル名を変更 */
    QDir dir(BasicSet::tmpDirectory);
    dir.rename(oldFileName, newFileName);
    selectedItems().takeAt(0)->setText(0, newFileName);

    /* リストの変更 */
    if(newFileName.contains(".txt")){
        QProcess *process = scriptList.value(oldFileName);
        scriptList.remove(oldFileName);
        scriptList.insert(newFileName, process);
    }
    else if(newFileName.contains(".csv")){
        sheetList.remove(oldFileName);
        sheetList.insert(newFileName);
    }
    else{
        otherList.remove(oldFileName);
        otherList.insert(newFileName);
    }
}

void FileTree::removeFile()
{
    /* ファイル名ではなく、項目名(Script,Sheet,Other)が選択された場合は無視 */
    if(selectedItems().takeAt(0)->parent() == nullptr) return;

    const QString parentTitle = selectedItems().takeAt(0)->parent()->text(0);                  //選択されたファイルの項目名
    const QString selectedFile = selectedItems().takeAt(0)->text(0);                           //選択されたファイルの名前
    const QString message = "Are you sure you want to remove this \"" + selectedFile + "\" ??   "; //ダイアログメッセージ

    /* 確認のためのメッセージボックス */
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "remove",
                                                              message,
                                                              QMessageBox::Yes | QMessageBox::No);
    /* 確認してNoまたは×ボタンが押されたらなら無効 */
    if(reply == QMessageBox::No) return;

    /* リストの削除 */
    if(parentTitle == "Script"){
        scriptList.remove(selectedFile);
        if(scriptList.size() <= currentScriptIndex) currentScriptIndex--;
    }
    else if(parentTitle == "Sheet"){
        sheetList.remove(selectedFile);
        if(sheetList.size() <= currentSheetIndex) currentSheetIndex--;
    }
    else if(parentTitle == "Other"){
        otherList.remove(selectedFile);
        if(otherList.size() <= currentOtherIndex) currentOtherIndex--;
    }

    /* ファイルの削除 */
    QDir dir(BasicSet::tmpDirectory);
    dir.remove(selectedFile);

    /* リストからファイルを削除 */
    selectedItems().takeAt(0)->parent()->removeChild(selectedItems().takeAt(0));
}

/* ファイルの保存 */
void FileTree::saveFile()
{
    /* ファイル名以外が選択された場合は無視 */
    if(selectedItems().takeAt(0)->parent() == nullptr) return;

    /* ディレクトリダイアログの表示と保存するフォルダーのフルパス取得 */
    const QString pathForSave = QFileDialog::getExistingDirectory(this);

    /* ディレクトリが選択されなければ無効 */
    if(pathForSave.isEmpty()) return;

    /* 選択されたファイルの名前 */
    const QString fileName = this->selectedItems().takeAt(0)->text(0);

    /* ファイルをコピーして保存 */
    QFile::copy(BasicSet::tmpDirectory + fileName, pathForSave + "/" + fileName);

    /* 保存したファイルをTreeから削除してよいかのメッセージボックス */
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "remove",
                                                              "Do you remove \"" + fileName + "\" from the file tree??",
                                                              QMessageBox::Yes | QMessageBox::No);

    /* replyがokならTreeから保存したファイルを削除 */
    if(reply == QMessageBox::No)
        return;
    else
    {
        /* 削除するファイルの項目名 */
        const QString parentTitle = selectedItems().takeAt(0)->parent()->text(0);
        /* ディレクトリとTreeからファイルの削除 */
        QDir dir(BasicSet::tmpDirectory);
        dir.remove(fileName);
        selectedItems().takeAt(0)->parent()->removeChild(selectedItems().takeAt(0));
        /*　リストから削除 */
        if(parentTitle == "Script"){
            scriptList.remove(fileName);
            if(scriptList.size() <= currentScriptIndex) currentScriptIndex--;
        }
        else if(parentTitle == "Sheet"){
            sheetList.remove(fileName);
            if(sheetList.size() <= currentSheetIndex) currentSheetIndex--;
        }
        else if(parentTitle == "Other"){
            otherList.remove(fileName);
            if(otherList.size() <= currentOtherIndex) currentOtherIndex--;
        }
    }

}











