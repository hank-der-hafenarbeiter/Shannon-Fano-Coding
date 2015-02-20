#include "symbol.h"



Symbol::Symbol(QChar p_sym):sym(p_sym),count(-1),prob(0),code(""){}


bool Symbol::operator < (const Symbol& str) const
{
        return (str.prob < prob);
}

bool Symbol::operator == (const QChar& p_sym) const
{
        return (p_sym == sym);
}

bool Symbol::operator == (const Symbol p_sym) const
{
    return (p_sym.sym == sym);
}

double Symbol::operator+(double i)
{
    return i + prob;
}

void Symbol::operator ++ (const int i)
{
    count += i;
}
