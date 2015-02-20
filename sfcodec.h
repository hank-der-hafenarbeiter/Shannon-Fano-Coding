#ifndef SFCODEC_H
#define SFCODEC_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QChar>
#include <QTableWidget>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextCursor>
#include <QChar>
#include <QGraphicsScene>
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <functional>
#include <bitset>

#include "sfvector.h"


class symbol;

class SFCodec
{
public:
    explicit SFCodec(QTextEdit* p_inputField = 0);

    QString encode();
    QString toBin();

    void updateIndex(); //calculate the code

    void updateTable(QTableWidget* table); //erneuert die informationen in der tabelle
    void updateStatus(QStatusBar* status); //erneuert die nachricht in der status leiste
    QImage getTreeView(int width, int height);


private:
    SFVector index;
    QString inputText;
    QTextEdit* inputField;
    QString outputText;
    QString outputBin;
};







#endif // SFCODEC_H
