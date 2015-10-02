#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include <memory>

#include "sfcodec.h"
#include "sftreenode.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_inputField_textChanged();
    void on_stepButton_clicked();
    void on_prevStepButton_clicked();
    void on_autoStepCheck_clicked();
    void on_smallStepCheck_clicked();
private:
    Ui::MainWindow *ui;
    SFCodec* codec;
    QString textBuffer; //for some reason the SIGNAL textChanged is emitted multiple time. textBuffer is needed for a workaround

    std::shared_ptr<SFTreeNode> codeTree;
};

#endif // MAINWINDOW_H
