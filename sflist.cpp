#include "sflist.h"

SFList::SFList():
    QList<Symbol>()
{

}


SFList::SFList(QList<Symbol> list) :
    QList<Symbol>(list)
{
}



/**
 * @brief SFList::sum sums the probability of all symbols in this list
 * @return return propability of all symbols in this list
 */
inline double SFList::sum() const
{
    double t_sum = 0;

    for(auto entry:*this)
        t_sum += entry.getProb();
    return t_sum;
}

/**
 * @brief SFList::sum return sum of probabilities of symbols in [it1,it2)
 * @param it1 SFList::iterator first element to be summed
 * @param it2 SFList::iterator to behind the last element to be summed
 * @return sum of probabilities between the two iterators
 */
inline double SFList::sum(const SFList::iterator it1, const SFList::iterator it2)
{

    double sum = 0;
    for(SFList::iterator i = it1; i != it2; i++)
    {
        sum += i->getProb();
    }
    return sum;
}

/**
 * @brief SFList::split split list [it1,it2) so that both have an equal sum of propabilities
 * @param it1 SFList::iterator pointing to the first element of the range
 * @param it2 SFList::iterator to behind the last element of the range
 * @return SFList::iterator iter so that sum[it1,iter) == sum[iter,it2)
 */
SFList::iterator SFList::split(const SFList::iterator it1, const SFList::iterator it2) //There is an exact solution to this problem in pseudo-linear time but i chose an easier heuristic that grants adequate results
{

    qDebug() << "list size:" << it2-it1;
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
