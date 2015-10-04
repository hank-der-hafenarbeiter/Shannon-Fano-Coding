#ifndef SFCODEC_H
#define SFCODEC_H

#include <iostream>
#include <QObject>
#include <QVector>
#include <QString>
#include <QChar>
#include <QLabel>
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

#include "sflist.h"
#include "sftreenode.h"


class symbol;

/**
 * \class SFCodec
 * @brief Implementation of the Shannon Fano coding
 *
 * This class generates an index of all symbols in the
 * source text and it's correspondent code according to
 * the Shannnon Fano coding
 *
 */

class SFCodec
{
public:
    explicit SFCodec(QTextEdit* p_inputField = 0);

    QString encode();
    QString toBin();

    void updateIndex(); //calculate the code
    SFList getIndex(){return index;}


    void updateTable(QTableWidget* table) const; //erneuert die informationen in der tabelle
    void updateStatus(QStatusBar* status) const; //erneuert die nachricht in der status leiste


private:
    void updateIndexHelper(const SFList::iterator it1, const SFList::iterator it2);

    SFList index;
    QString inputText;
    QTextEdit* inputField;
    QString outputText;
    QString outputBin;
};







#endif // SFCODEC_H
