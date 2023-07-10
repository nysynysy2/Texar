#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QImageReader>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QFontComboBox>
#include <QTransform>
#include <QLabel>
#include <QToolButton>
#include <QFontDatabase>
#include <QColorDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QActionGroup>
#include <QTextCursor>
#include <QTextList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showNewFile();
    void showOpenFile();
    void loadFile();
    void showPrintText();
    void showPrintImage();
    void showZoomIn();
    void showZoomOut();
    void showSaveFile();
    void showSaveAsFile();
    void showRotate90();
    void showRotate180();
    void showRotate270();
    bool event(QEvent* ev)override;
    bool saved;
private:
    Ui::MainWindow *ui;
    QImage img;
    QString fileName;
    QLabel* fontLabel1;
    QFontComboBox* fontComboBox;
    QLabel* fontLabel2;
    QComboBox* sizeComboBox;
    QToolButton* boldBtn;
    QToolButton* italiBtn;
    QToolButton* underlineBtn;
    QToolButton* colorBtn;
    QColor color;
    QLabel* listLabel;
    QActionGroup* actionGroup;
    QComboBox* listComboBox;
    QAction* centerAction;
    QAction* leftAction;
    QAction* rightAction;
    QAction* justifyAction;
    void initToolBars();
};
#endif // MAINWINDOW_H
