#ifndef SYMBOL_H
#define SYMBOL_H

#include <QChar>
#include <QString>
#include <QDebug>


class Symbol {
public:
    Symbol(QChar p_sym = QChar());
    Symbol(const Symbol& p_sym){sym = p_sym.getSym(); count = p_sym.getCount(); prob = p_sym.getProb(); code = p_sym.getCode();};
    Symbol(Symbol& p_sym){sym = p_sym.getSym(); count = p_sym.getCount(); prob = p_sym.getProb(); code = p_sym.getCode();};

    void setSym(QChar p_sym){sym = p_sym;};
    void setCount(int p_count){count = p_count;};
    void setProb(double p_prob){prob = p_prob;};
    void setCode(QString p_code){code = p_code;};
    void appendCode(QString p_code){code.append(p_code);};

    QChar getSym() const {return sym;};
    int getCount() const {return count;};
    double getProb() const {return prob;};
    QString getCode() const {return code;};

    bool operator < (const Symbol& str) const;
    bool operator == (const QChar& p_sym) const;
    bool operator == (const Symbol p_sym) const;
    double operator+(double i);
    void operator ++ (const int i);

private:
    QChar sym;		//speichert das Symbol
    int count;		//speicher die häufigkeit
    double prob;	//speicher wahrscheinlichkeit
    QString code;		//das codierte gegenstück
};



QDebug operator<<(QDebug dbg, const Symbol &sym);


#endif // SYMBOL_H
