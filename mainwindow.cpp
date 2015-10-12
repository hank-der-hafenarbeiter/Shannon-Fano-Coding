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
    ui->statusBar->show();
    ui->statusBar->showMessage("Ready",2000);

    QObject::connect(ui->StepBotton, SIGNAL(clicked()),this,SLOT(on_stepButton_clicked()));
    QObject::connect(ui->PrevStepBottom, SIGNAL(clicked()), this, SLOT(on_prevStepButton_clicked()));
    QObject::connect(ui->autoStepCheck, SIGNAL(clicked()), this, SLOT(on_autoStepCheck_clicked()));
    QObject::connect(ui->smallStepCheck, SIGNAL(clicked()), this, SLOT(on_smallStepCheck_clicked()));

}

void MainWindow::on_inputField_textChanged()
{
    if(textBuffer != ui->inputField->toPlainText())
    {
        codec->updateIndex();
        ui->outputField->setText(codec->encode());
        ui->textbinary_field->setText(codec->toBin());
        codec->updateStatus(ui->statusBar);
        codec->updateTable(ui->key_table);
        textBuffer = ui->inputField->toPlainText();

        codeTree = std::make_shared<SFTreeNode>(codec->getIndex());
        if(ui->autoStepCheck->isChecked())
            while(codeTree->step());
        ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
    }
}

void MainWindow::on_stepButton_clicked()
{
    if(codeTree)
    {
        if(ui->smallStepCheck->isChecked())
            codeTree->smallStep();
        else
            codeTree->step();
        ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
    }
}
void MainWindow::on_prevStepButton_clicked()
{
    if(codeTree)
    {
        codeTree->step_back();
        ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
    }
}

void MainWindow::on_autoStepCheck_clicked()
{
    ui->StepBotton->setDisabled(ui->autoStepCheck->isChecked());
    ui->PrevStepBottom->setDisabled(ui->autoStepCheck->isChecked());
    ui->smallStepCheck->setDisabled(ui->autoStepCheck->isChecked());

    if(codeTree && ui->autoStepCheck->isChecked())
    {
        while(codeTree->step());
        ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
    }
    ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
}

void MainWindow::on_smallStepCheck_clicked()
{
    ui->autoStepCheck->setDisabled(ui->smallStepCheck->isChecked());


    ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
}

MainWindow::~MainWindow()
{
    delete codec;
    delete ui;
}
