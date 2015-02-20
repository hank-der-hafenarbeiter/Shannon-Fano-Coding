#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "sfcodec.h"


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
private:
    Ui::MainWindow *ui;
    SFCodec* codec;
    QString textBuffer; //for some reason the SIGNAL textChanged is emitted multiple time. textBuffer is needed for a workaround
};

#endif // MAINWINDOW_H
