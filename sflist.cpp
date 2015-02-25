#include "sflist.h"

SFList::SFList():
    QList<Symbol>()
{

}


SFList::SFList(QList<Symbol> list) :
    QList<Symbol>(list)
{
}




double SFList::sum()
{
    double t_sum = 0;
    std::for_each(begin(),end(), [&t_sum](Symbol entry){t_sum += entry.getProb();});
    return t_sum;
}

void SFList::split(SFList &vec_left, SFList &vec_right) //There is an exact solution to this problem in pseudo-linear time but i chose an easier heuristic that grants adequate results
{
    vec_left.append(vec_right);
    if(vec_left.size())
    {
        for(auto iter = vec_left.begin()+1; iter < vec_left.end(); iter += 2)   //put every other element in the right vector to get a good approximation of the balanced split
        {
            vec_right.append(*iter);
            vec_left.erase(iter);
        }

        double  min_diff = 0,                                   //maximum diff found so far
                gap = fabs(vec_left.sum() - vec_right.sum());   //gap between vec_left and vec_right

        SFList::iterator iter = NULL;   //first element that fits the gap
        Symbol symbol;

        iter = std::find_if_not(vec_left.begin(), vec_left.end(), [&](const Symbol sym){return sym.getProb() > gap/2;});


        while(iter != vec_left.end())   //improve approximation a bit by finding the element that fits the gab best.
        {
            symbol = *iter;

            vec_left.erase(iter);

            iter = std::find_if(vec_right.begin(), vec_right.end(), [&](const Symbol sym){return sym.getProb() < symbol.getProb();});   //search for the first item with a lower probability

            if(iter != vec_right.end()) //if search yielded a result
                vec_right.insert(iter, symbol);     //insert key before that maintaining a sorted list
            else
                vec_right.append(symbol);
            gap = fabs(vec_left.sum() - vec_right.sum());   //calc new gap
            iter = std::find_if_not(vec_left.begin(), vec_left.end(), [&](const Symbol sym){return sym.getProb() > gap/2;});    //find the next element that fits that gap
        }






     /*   double diff1 = fabs(vec_left.sum() - vec_right.sum()), diff2 = 0; //the difference between the sum of probabilities during the previous and current iteration

        do
        {
            vec_left > vec_right;
            diff2 = diff1;
            diff1 = fabs(vec_left.sum() - vec_right.sum());
        }while((diff1-diff2) < 0); //compare two diffs to check if the iteration improved or worsened the result

        //the loop continues until it worsened the difference once so we have to roll back once
        vec_left < vec_right;

        */
    }
}
