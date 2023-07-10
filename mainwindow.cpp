#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(820,600);
    this->setWindowTitle("Texar");
    this->setWindowIcon(QIcon(":/image/icon.png"));
    this->saved = true;
    ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());
    ui->imageLabel->setScaledContents(true);
    if(img.load("image.png"))
    {
        ui->imageLabel->setPixmap(QPixmap::fromImage(img));
    }

    initToolBars();

    connect(ui->actionNew,&QAction::triggered,this,&MainWindow::showNewFile);

    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::showOpenFile);

    connect(ui->actionPrintText,&QAction::triggered,this,&MainWindow::showPrintText);

    connect(ui->actionPrintImage,&QAction::triggered,this,&MainWindow::showPrintImage);

    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::showSaveFile);

    connect(ui->actionSaveAs,&QAction::triggered,this,&MainWindow::showSaveAsFile);

    connect(ui->actionZoomIn,&QAction::triggered,this,&MainWindow::showZoomIn);

    connect(ui->actionZoomOut,&QAction::triggered,this,&MainWindow::showZoomOut);

    connect(ui->actionRotate90,&QAction::triggered,this,&MainWindow::showRotate90);

    connect(ui->actionRotate180,&QAction::triggered,this,&MainWindow::showRotate180);

    connect(ui->actionRotate270,&QAction::triggered,this,&MainWindow::showRotate270);

    connect(ui->textEdit,&QTextEdit::textChanged,[=](){this->saved = false;});

    connect(ui->actionHorizontal,&QAction::triggered,[=](){
        if(img.isNull())return;
        QTransform matrix;
        matrix.scale(-1,1);

        img = img.transformed(matrix);
        ui->imageLabel->setPixmap(QPixmap::fromImage(img));
    });

    connect(ui->actionVertical,&QAction::triggered,[=](){
        if(img.isNull())return;
        QTransform matrix;
        matrix.scale(1,-1);

        img = img.transformed(matrix);
        ui->imageLabel->setPixmap(QPixmap::fromImage(img));
    });

    connect(ui->actionUndo,&QAction::triggered,[=](){
        if(!(ui->actionUndo->isEnabled()))
            return;

        ui->textEdit->document()->undo();
    });

    connect(ui->actionRedo,&QAction::triggered,[=](){
        if(!(ui->actionRedo->isEnabled()))
            return;

        ui->textEdit->document()->redo();
    });

    connect(fontComboBox,&QFontComboBox::currentFontChanged,[=](const QFont& f){
        QTextCharFormat format;
        format.setFont(f);
        ui->textEdit->mergeCurrentCharFormat(format);
    });
    connect(sizeComboBox,&QComboBox::currentTextChanged,[=](const QString& str){
        QTextCharFormat format;
        format.setFontPointSize(str.toFloat());
        ui->textEdit->mergeCurrentCharFormat(format);
    });
    connect(colorBtn,&QPushButton::clicked,[=](){
        color = QColorDialog::getColor(Qt::white,this,"Select Color");
        if(color.isValid())
        {
            QTextCharFormat format;
            format.setForeground(color);
            ui->textEdit->mergeCurrentCharFormat(format);
        }
    });

    connect(boldBtn,&QToolButton::clicked,[=](bool checked){
        QTextCharFormat format;
        format.setFontWeight(checked?QFont::Bold:QFont::Normal);
        ui->textEdit->mergeCurrentCharFormat(format);
    });

    connect(italiBtn,&QToolButton::clicked,[=](bool checked){
        QTextCharFormat format;
        format.setFontItalic(checked);
        ui->textEdit->mergeCurrentCharFormat(format);
    });

    connect(underlineBtn,&QToolButton::clicked,[=](bool checked){
        QTextCharFormat format;
        format.setFontUnderline(checked);
        ui->textEdit->mergeCurrentCharFormat(format);
    });

    connect(ui->actionExit,&QAction::triggered,[=](){ emit this->close(); });

    connect(ui->textEdit->document(),&QTextDocument::undoAvailable,ui->actionUndo,&QAction::setEnabled);

    connect(ui->textEdit->document(),&QTextDocument::redoAvailable,ui->actionRedo,&QAction::setEnabled);

    connect(this->listComboBox,&QComboBox::activated,[=](int index){
        QTextCursor cursor = ui->textEdit->textCursor();

        if(index)
        {
            QTextListFormat::Style style = QTextListFormat::ListDisc;
            switch(index)
            {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
            }
            cursor.beginEditBlock();

            QTextBlockFormat blockFormat = cursor.blockFormat();
            QTextListFormat listFormat;

            if(cursor.currentList())
            {
                listFormat = cursor.currentList()->format();
            }
            else
            {
                listFormat.setIndent(blockFormat.indent());
                blockFormat.setIndent(0);
                cursor.setBlockFormat(blockFormat);
            }
            listFormat.setStyle(style);
            cursor.createList(listFormat);

            cursor.endEditBlock();
        }
        else
        {
            QTextBlockFormat blockFormat;
            blockFormat.setObjectIndex(-1);
            cursor.mergeBlockFormat(blockFormat);
        }
    });

    connect(ui->textEdit,&QTextEdit::cursorPositionChanged,[=](){
        if(ui->textEdit->alignment() == Qt::AlignLeft)
            this->leftAction->setChecked(true);
        if(ui->textEdit->alignment() == Qt::AlignRight)
            this->rightAction->setChecked(true);
        if(ui->textEdit->alignment() == Qt::AlignCenter)
            this->centerAction->setChecked(true);
        if(ui->textEdit->alignment() == Qt::AlignJustify)
            this->justifyAction->setChecked(true);
    });

    connect(this->actionGroup,&QActionGroup::triggered,[=](QAction* action){
        if(action == this->leftAction)
            ui->textEdit->setAlignment(Qt::AlignLeft);
        if(action == this->rightAction)
            ui->textEdit->setAlignment(Qt::AlignRight);
        if(action == this->centerAction)
            ui->textEdit->setAlignment(Qt::AlignCenter);
        if(action == this->justifyAction)
            ui->textEdit->setAlignment(Qt::AlignJustify);
    });

    connect(ui->actionAbout,&QAction::triggered,this,&QApplication::aboutQt);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initToolBars()
{
//    ui->fileToolBar->addAction(ui->actionOpen);
//    ui->fileToolBar->addAction(ui->actionNew);
//    ui->fileToolBar->addAction(ui->actionSave);
//    ui->fileToolBar->addAction(ui->actionSaveAs);
//    ui->fileToolBar->addSeparator();
//    ui->fileToolBar->addAction(ui->actionPrintText);
//    ui->fileToolBar->addAction(ui->actionPrintImage);
//    ui->fileToolBar->addSeparator();
//    ui->fileToolBar->addAction(ui->actionExit);

//    ui->editToolBar->addAction(ui->actionCopy);
//    ui->editToolBar->addAction(ui->actionCut);
//    ui->editToolBar->addAction(ui->actionPaste);
//    ui->editToolBar->addSeparator();
//    ui->editToolBar->addAction(ui->actionZoomIn);
//    ui->editToolBar->addAction(ui->actionZoomOut);

//    ui->rotateToolBar->addAction(ui->actionRotate90);
//    ui->rotateToolBar->addAction(ui->actionRotate180);
//    ui->rotateToolBar->addAction(ui->actionRotate270);


    fontLabel1 = new QLabel("  Font:");
    fontComboBox = new QFontComboBox;
    fontComboBox->setFontFilters(QFontComboBox::ScalableFonts);
    fontComboBox->setEditable(false);

    fontLabel2 = new QLabel("  Size:");
    sizeComboBox = new QComboBox;
    foreach(int size,QFontDatabase::standardSizes())
    {
        sizeComboBox->addItem(QString::number(size));
    }

    boldBtn = new QToolButton;
    boldBtn->setIcon(QIcon(":/image/bold.png"));
    boldBtn->setCheckable(true);

    italiBtn = new QToolButton;
    italiBtn->setIcon(QIcon(":/image/italic.png"));
    italiBtn->setCheckable(true);

    underlineBtn = new QToolButton;
    underlineBtn->setIcon(QIcon(":/image/underline.png"));
    underlineBtn->setCheckable(true);

    colorBtn = new QToolButton;
    colorBtn->setIcon(QIcon(":/image/palette.png"));


    listLabel = new QLabel("List:");
    listComboBox = new QComboBox;

    listComboBox->addItem("Standard");
    listComboBox->addItem("List Format: Disc");
    listComboBox->addItem("List Format: Circle");
    listComboBox->addItem("List Format: Square");
    listComboBox->addItem("List Format: Decimal");
    listComboBox->addItem("List Format: Lower Alphabet");
    listComboBox->addItem("List Format: Upper Alphabet");
    listComboBox->addItem("List Format: Lower Roman");
    listComboBox->addItem("List Format: Upper Roman");


    leftAction = new QAction();
    leftAction->setIcon(QIcon(":/image/left-align.png"));
    leftAction->setCheckable(true);
    rightAction = new QAction();
    rightAction->setIcon(QIcon(":/image/right-align.png"));
    rightAction->setCheckable(true);
    centerAction = new QAction();
    centerAction->setIcon(QIcon(":/image/center-align.png"));
    centerAction->setCheckable(true);
    justifyAction = new QAction();
    justifyAction->setIcon(QIcon(":/image/justific-ation.png"));
    justifyAction->setCheckable(true);

    actionGroup = new QActionGroup(this);
    actionGroup->addAction(leftAction);
    actionGroup->addAction(rightAction);
    actionGroup->addAction(centerAction);
    actionGroup->addAction(justifyAction);

    ui->listToolBar->addWidget(listLabel);
    ui->listToolBar->addWidget(listComboBox);
    ui->listToolBar->addActions(actionGroup->actions());

    ui->fontToolBar->addWidget(fontLabel1);
    ui->fontToolBar->addWidget(fontComboBox);
    ui->fontToolBar->addWidget(fontLabel2);
    ui->fontToolBar->addWidget(sizeComboBox);
    ui->fontToolBar->addSeparator();
    ui->fontToolBar->addWidget(boldBtn);
    ui->fontToolBar->addWidget(italiBtn);
    ui->fontToolBar->addWidget(underlineBtn);
    ui->fontToolBar->addSeparator();
    ui->fontToolBar->addWidget(colorBtn);

//    ui->doToolBar->addAction(ui->actionUndo);
//    ui->doToolBar->addAction(ui->actionRedo);
}

void MainWindow::showNewFile()
{
    MainWindow* newWindow = new MainWindow;
    newWindow->show();
}

void MainWindow::showOpenFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        if(ui->textEdit->document()->isEmpty())
        {
            this->loadFile();
        }
        else
        {
            MainWindow* newWindow = new MainWindow;
            newWindow->fileName = this->fileName;
            newWindow->loadFile();
            newWindow->show();
        }
    }
}

void MainWindow::loadFile()
{
    QFile file(this->fileName);
    QImageReader qir(this->fileName);
    if(!qir.canRead())
    {
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QTextStream ts(&file);
            ui->textEdit->setText(ts.readAll());
        }
    }
    else
    {
        ui->imageLabel->setPixmap(QPixmap(this->fileName));
        this->img = QImage(this->fileName);
        this->fileName = QString();
    }
    file.close();
}

void MainWindow::showPrintText()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if(printDialog.exec())
    {
        QTextDocument* doc = ui->textEdit->document();
        doc->print(&printer);
    }
}

void MainWindow::showPrintImage()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer,this);
    if(printDialog.exec())
    {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = img.size();
        size.scale(rect.size(),Qt::KeepAspectRatio);
        painter.setViewport(rect.x(),rect.y(),size.width(),size.height());
        painter.setWindow(img.rect());
        painter.drawImage(0,0,img);
    }
}

void MainWindow::showSaveFile()
{
    if(fileName.isEmpty())
    {
        this->showSaveAsFile();
    }
    else
    {
        QFile file(this->fileName);
        if(file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::ReadWrite))
        {
            QTextStream textStream(&file);
            textStream << ui->textEdit->toPlainText();
            saved = true;
        }
        file.close();
    }
}

void MainWindow::showSaveAsFile()
{
    QString path = QFileDialog::getSaveFileName(this,QString(),QString(),QString("Text(*.txt);;C++(*.cpp);;Java(*.java);;Batch(*bat);;Shell(*.sh)"));
    QFile file(path);
    if(file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::ReadWrite))
    {
        QTextStream textStream(&file);
        textStream << ui->textEdit->toPlainText();
        saved = true;
    }
    file.close();
}

void MainWindow::showZoomIn()
{
    if(img.isNull())return;
    QTransform matrix;
    matrix.scale(2,2);

    img = img.transformed(matrix);
    ui->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showZoomOut()
{
    if(img.isNull())return;
    QTransform matrix;
    matrix.scale(0.5,0.5);

    img = img.transformed(matrix);
    ui->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showRotate90()
{
    if(img.isNull())return;
    QTransform matrix;
    matrix.rotate(90);

    img = img.transformed(matrix);
    ui->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showRotate180()
{
    if(img.isNull())return;
    QTransform matrix;
    matrix.rotate(180);

    img = img.transformed(matrix);
    ui->imageLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::showRotate270()
{
    if(img.isNull())return;
    QTransform matrix;
    matrix.rotate(270);

    img = img.transformed(matrix);
    ui->imageLabel->setPixmap(QPixmap::fromImage(img));
}

bool MainWindow::event(QEvent* ev)
{
    if(ev->type() == QEvent::Close)
    {
        auto ret = QMessageBox::question(this,"Exit","Are you sure to exit?");
        if(ret == QMessageBox::Yes)
        {
            if(!saved)
            {
                switch(QMessageBox::question(this,"Save","The file has been changed. Do you want to save?",QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel))
                {
                case QMessageBox::Save:
                    emit ui->actionSave->triggered();
                    exit(0);
                case QMessageBox::Discard:
                    exit(0);
                case QMessageBox::Cancel:
                    ev->ignore();
                    return true;
                default:
                    ev->ignore();
                    return true;
                }
            }
        }
        else
        {
            ev->ignore();
            return true;
        }
    }
    return QMainWindow::event(ev);
}
