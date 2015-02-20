#ifndef SFVector_H
#define SFVector_H

#include <QVector>
#include <QObject>
#include "symbol.h"

class SFVector : public QVector<Symbol>
{
public:
    explicit SFVector(QObject *parent = 0);


    double sum();
private:


};

#endif // SFVector_H
