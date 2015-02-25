#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    codec = new SFCodec(ui->inputField);
    QWidget::showMaximized();

    QImage temp(ui->treeView->width(), ui->treeView->height(), QImage::Format_ARGB32);
    temp.fill(QColor(255,255,255));
    ui->treeView->setPixmap(QPixmap::fromImage(temp));
    ui->treeView->show();

}

void MainWindow::on_inputField_textChanged()
{
    if(textBuffer != ui->inputField->toPlainText())
    {
        codec->updateIndex();
        ui->outputField->setText(codec->encode());
        ui->textbinary_field->setText(codec->toBin());
        codec->updateTable(ui->key_table);
        textBuffer = ui->inputField->toPlainText();


        QImage temp(ui->treeView->width(), ui->treeView->height(), QImage::Format_ARGB32);
        temp.fill(QColor(255,255,255));
        ui->treeView->setPixmap(QPixmap::fromImage(temp));
        ui->treeView->setPixmap(QPixmap::fromImage(codec->getTreeView(ui->treeView->width(), ui->treeView->height())));
    }
}

MainWindow::~MainWindow()
{
    delete codec;
    delete ui;
}
