#ifndef SFVector_H
#define SFVector_H

#include <iostream>
#include <cassert>
#include <QVector>
#include <QObject>
#include "symbol.h"

/**
 *\class
 * @brief The SFList class is an expansion of QList adding functionality needed for SFCodec
 *
 * The SFList expands QList adding "<<" und ">>" operatoren, a sum and a split function.
 */
class SFList : public QList<Symbol>
{
public:
    explicit SFList();
    SFList(QList<Symbol> list);

    void operator>>(SFList& vec_right){vec_right.prepend(this->last()); this->pop_back();}        //takes last element and puts it at the beginning of vec_right then deletes in this vector
    void operator<<(SFList& vec_right){this->append(vec_right.first()); vec_right.pop_front();}    //takes the first element of the other vector and adds it at the end of this

    double sum() const;


    static SFList::iterator split(const SFList::iterator it1, const SFList::iterator it2);
    static inline double sum(const SFList::iterator it1, const SFList::iterator it2);
private:


};

#endif // SFVector_H
