#include "sflist.h"

SFList::SFList():
    QList<Symbol>()
{

}


SFList::SFList(QList<Symbol> list) :
    QList<Symbol>(list)
{
}




inline double SFList::sum()
{
    double t_sum = 0;

    for(auto entry:*this)
        t_sum += entry.getProb();
    return t_sum;
}

inline double SFList::sum(const SFList::iterator it1, const SFList::iterator it2)
{
    double sum = 0;
    for(SFList::iterator i = it1; i != it2; i++)
    {
        sum += i->getProb();
    }
    return sum;
}

SFList::iterator SFList::split(const SFList::iterator it1, const SFList::iterator it2) //There is an exact solution to this problem in pseudo-linear time but i chose an easier heuristic that grants adequate results
{
    SFList::iterator iter = it2;

    if(it2-it1 > 1)
    {
        iter = it1;
        double half = SFList::sum(it1, it2)/2.0;
        double sum = 0;


        while(sum < half) //increase pointer until at or to the right of the best balance
        {
            sum += iter->getProb();
            iter++;
        }
        if( (sum-half) > (half-SFList::sum(it1, iter-1)))   //check if best balance is one to the left
            iter--;
    }
    return iter;
}
