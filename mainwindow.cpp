#include "mainwindow.h"
#include "ui_mainwindow.h"


/**
 * @brief MainWindow::MainWindow sets up the UI at the start of the program.
 * @param parent is a pointer to the parent widged
 *
 * This constructor sets up all the parameters at the programstart.
 * It maximizes the window. Initializes all the textfields, the table
 * and the QImage which will later show the code tree.
 * It also sets up the signals and slots
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    codec = std::unique_ptr<SFCodec>(new SFCodec(ui->inputField));
    QWidget::showMaximized();

    QImage temp(ui->treeView->width(), ui->treeView->height(), QImage::Format_ARGB32);
    temp.fill(QColor(255,255,255));
    ui->treeView->setPixmap(QPixmap::fromImage(temp));
    ui->treeView->show();
    ui->statusBar->show();
    ui->statusBar->showMessage("Ready",2000);

    QObject::connect(ui->StepButton, SIGNAL(clicked()),this,SLOT(on_stepButton_clicked()));
    QObject::connect(ui->PrevStepButton, SIGNAL(clicked()), this, SLOT(on_prevStepButton_clicked()));
    QObject::connect(ui->autoStepCheck, SIGNAL(clicked()), this, SLOT(on_autoStepCheck_clicked()));
    QObject::connect(ui->smallStepCheck, SIGNAL(clicked()), this, SLOT(on_smallStepCheck_clicked()));

}

/**
 * @brief MainWindow::on_inputField_textChanged is called when the inputText is changed. It updates everything
 *
 * If the inputText changed this function tells the Codec to update and updates all the textFields,
 * the table and the code tree
 */
void MainWindow::on_inputField_textChanged()
{
    if(textBuffer != ui->inputField->toPlainText())
    {
        codec->updateIndex();
        ui->outputField->setText(codec->encode());
        updateBin();
        updateStatus();
        updateTable();
        textBuffer = ui->inputField->toPlainText();

        codeTree = std::make_shared<SFTreeNode>(codec->getIndex());
        if(ui->autoStepCheck->isChecked())
            while(codeTree->step());
        ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
    }
}

/**
 * @brief MainWindow::on_stepButton_clicked if called when the "Next Step" button is clicked
 *
 * It tells the codeTree to execute the next step in codeGeneration and then updates the
 * QImage that displays the tree
 */
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

/**
 * @brief MainWindow::on_prevStepButton_clicked takes a step back
 *
 * Rolls back the last step in the code generation and then updates the
 * QImage that displays the tree
 */
void MainWindow::on_prevStepButton_clicked()
{
    if(codeTree)
    {
        codeTree->step_back();
        ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
    }
}

/**
 * @brief MainWindow::on_autoStepCheck_clicked called when the checkbox for auto stepping was clicked
 *
 * Sets up the programs behaviour according to the new state of the checkBox
 *
 */
void MainWindow::on_autoStepCheck_clicked()
{
    ui->StepButton->setDisabled(ui->autoStepCheck->isChecked());
    ui->PrevStepButton->setDisabled(ui->autoStepCheck->isChecked());
    ui->smallStepCheck->setDisabled(ui->autoStepCheck->isChecked());

    if(codeTree && ui->autoStepCheck->isChecked())
    {
        while(codeTree->step())
        {
        }
        updateTreeView();
    }
    else
    {
        while(codeTree->step_back())
        {
        }
        updateTreeView();
    }
    ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
}

/**
 * @brief MainWindow::on_smallStepCheck_clicked called when the checkbox for small stepping was clicked
 *
 * Sets up the program's behaviour according to the new state of the checkbox
 */
void MainWindow::on_smallStepCheck_clicked()
{
    ui->autoStepCheck->setDisabled(ui->smallStepCheck->isChecked());
}

/**
 * @brief MainWindow::updateTable updates the table on the right side to represent the current state
 */
void MainWindow::updateTable()
{
    if(codec)
    {
        QTableWidget* table = ui->key_table;
        SFList index = codec->getIndex();

        table->setColumnCount(4);
        table->setRowCount(index.size());

        QString data;
        int row = 0;

        for(auto const entry:index)
        {
            data = entry.getSym();
            table->setItem(row,0,new QTableWidgetItem(data));

            data = QString::number(entry.getCount());
            table->setItem(row,1,new QTableWidgetItem(data));

            data = QString::number(entry.getProb());
            table->setItem(row,2,new QTableWidgetItem(data));

            data = entry.getCode();
            table->setItem(row,3,new QTableWidgetItem(data));

            row++;
        }
    }
}

void MainWindow::updateTreeView()
{
    ui->treeView->setPixmap(QPixmap::fromImage(SFTreeNode::drawTree(codeTree,ui->treeView->width(), ui->treeView->height())));
}

void MainWindow::updateBin()
{
    std::bitset<8> bitset;
    QString outputBin, inputText = ui->inputField->toPlainText();
    for(auto const character:inputText)
    {
        for(int i = 6; i > -1; i--)
            outputBin += QString::number((character.toLatin1() & (1 << i))!= 0);
    }
    ui->textbinary_field->setText(outputBin);
}

void MainWindow::updateStatus()
{
    QString message,
            outputText = ui->outputField->toPlainText(),
            outputBin = ui->textbinary_field->toPlainText();

    message = QString::number((double)outputText.length()*100/(double)outputBin.length()) + QString("%");
    ui->statusBar->showMessage(message);
}


MainWindow::~MainWindow()
{
    delete ui;
}
