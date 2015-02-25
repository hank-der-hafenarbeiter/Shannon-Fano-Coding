#ifndef SFVector_H
#define SFVector_H

#include <QVector>
#include <QObject>
#include "symbol.h"

class SFList : public QList<Symbol>
{
public:
    explicit SFList();
    SFList(QList<Symbol> list);

    void operator>(SFList& vec_right){vec_right.prepend(this->last()); this->pop_back();}        //takes last element and puts it at the beginning of vec_right then deletes in this vector
    void operator<(SFList& vec_right){this->append(vec_right.first()); vec_right.pop_front();}    //takes the first element of the other vector and adds it at the end of this

    double sum();

    static void split(SFList& vec_left, SFList& vec_right);
private:


};

#endif // SFVector_H
