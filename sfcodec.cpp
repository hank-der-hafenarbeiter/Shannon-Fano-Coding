#include "sfcodec.h"

SFCodec::SFCodec(QTextEdit* p_inputField) :
    index(),
    outputText(),
    outputBin()
{
    if(p_inputField != 0)
    {
        inputField = p_inputField;
        inputText = inputField->toPlainText();
    }
}

/**
 * @brief SFCodec::encode encodes the input text
 * @return QString of the encoded input text
 */
QString SFCodec::encode()
{
    outputText.clear();
    int i = -1;
    for(auto sym:inputText)
    {
        i = index.indexOf(sym);
        if(i >= 0 && i < index.size())
            outputText.append(index.at(i).getCode());
    }

    return outputText;
}
    //the list has to be sorted from highest to lowest probability
/**
 * @brief SFCodec::toBin gives a binary representation of the input text
 * @return QString of the binary representation
 */
QString SFCodec::toBin()
{
    outputBin.clear();
    std::bitset<8> bitset;

    for(auto const character:inputText)
    {
        bitset = std::bitset<8>(character.toLatin1());
        outputBin += QString::fromStdString(bitset.to_string());
        outputBin.remove(0,1);
    }

    return outputBin;
}

/**
 * @brief SFCodec::updateIndex updates the index
 *
 * Call this whenever the input text is changed.
 * This updates the index.
 */
void SFCodec::updateIndex()
{
    index.clear();

    inputText = inputField->toPlainText();
    if(inputText.length() == 0)
        return;

    int t_pos;
    for(auto t_char:inputText)
    {
        t_pos = index.indexOf(t_char);
        if(t_pos >= 0)
        {
            index[t_pos].operator ++(1);
        }
        else
        {
            index.push_back(Symbol(t_char));
            index.last().setCount(1);
        }
    }

    for(auto &sym:index)
        sym.setProb((double)sym.getCount()/(double)inputText.length());

    qSort(index.begin(),index.end()); //the list has to be sorted from highest to lowest probability
    updateIndexHelper(index.begin(), index.end());
}

/**
 * @brief SFCodec::updateIndexHelper creates the codes by recursivly calling itself
 * @param it1 SFList::iterator to the first relevant Symbol
 * @param it2 SFList::iterator to the address behind the last releveant Symbol
 * The SFList between the iterators has to be sorted!!!
 * This function calculates and adds a code to each character:
 * 1.) devide the vector into two vectors with equal probability
 * 2.) add a zero to the left and a one to the right vector
 * 3.) call this function recursivly for both parts of the list
 */
void SFCodec::updateIndexHelper(const SFList::iterator it1, const SFList::iterator it2)
{
    SFList::iterator mid = SFList::split(it1, it2);                     //(1)

    if(it1 != mid)
        std::for_each(it1, mid, [](Symbol& sym){sym.appendCode("0");}); //(2)
    if(it2 != mid)
        std::for_each(mid, it2, [](Symbol& sym){sym.appendCode("1");});

    if(std::distance(it1, mid) > 1)                                     //(3)
        updateIndexHelper(it1, mid);
    if(std::distance(mid, it2) > 1)
        updateIndexHelper(mid, it2);
}

/**
 * @brief SFCodec::updateTable modify the table showing the data of each symbol
 * @param table pointer to the QTableWidget of the mainwindow
 */
void SFCodec::updateTable(QTableWidget* table) const
{
    table->setColumnCount(4);
    table->setRowCount(index.size());

    QString data;
    int row = 0;

    for(auto const entry:index)
    {
            data = entry.getSym();
            table->setItem(row,0,new QTableWidgetItem(data));

            data = QString::number(entry.getCount());
            table->setItem(row,1,new QTableWidgetItem(data));

            data = QString::number(entry.getProb());
            table->setItem(row,2,new QTableWidgetItem(data));

            data = entry.getCode();
            table->setItem(row,3,new QTableWidgetItem(data));

            row++;
    }
}

/**
 * @brief SFCodec::updateStatus update the statusbar of the mainwindow
 * @param status pointer to the mainwindow's status bar
 */
void SFCodec::updateStatus(QStatusBar* status) const
{
    QString message;
    message = QString::number((double)outputText.length()*100/(double)outputBin.length()) + QString("%");
    status->showMessage(message);
}


