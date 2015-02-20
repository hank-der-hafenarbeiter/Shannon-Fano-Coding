#include "sfvector.h"

SFVector::SFVector(QObject *parent) :
    QVector<Symbol>()
{
}



double SFVector::sum()
{
    double t_sum = 0;
    std::for_each(begin(),end(), [&t_sum](Symbol entry){t_sum += entry.getProb();});
    return t_sum;
}
