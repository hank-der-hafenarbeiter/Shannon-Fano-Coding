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

QString SFCodec::encode()
{
    outputText.clear();
    int i = -1;
    std::for_each(inputText.begin(), inputText.end(), [&](QChar sym)
    {
        i = index.indexOf(sym);
        if(i >= 0 && i < index.size())
            outputText.append(index.at(i).getCode() + QString(" "));
    });

    return outputText;
}

//Returns a string representing how the input text would be stored in memory if it was a c_string (and not a QString/QTextEdit)
QString SFCodec::toBin()
{
    QString b_text;
    std::bitset<8> bitset;
    std::for_each(inputText.begin(), inputText.end(),[&](QChar character)
    {
        bitset = std::bitset<8>(character.toLatin1());
        b_text += QString::fromStdString(bitset.to_string());
        b_text += "  ";
    });

    return b_text;
}

void SFCodec::updateIndex()
{
    index.clear();  //dump old data (modifying it would be possible:    1. set all counts to zero
                    //                                                  2. calc new index
                    //                                                  3. delete all entries with count == zero)

    inputText = inputField->toPlainText();  //QTextEdit to QString
    if(inputText.length() == 0)
        return;

    QString wc_inputText = inputText; //working copy of inputText
    QChar t_symbol_buffer;            //holds single characters

    while(wc_inputText.size())
    {
        t_symbol_buffer = wc_inputText.at(0);                       //copy the next Symbol
        index.push_front(Symbol(t_symbol_buffer));                   //index it
        index[0].setCount(wc_inputText.count(t_symbol_buffer, Qt::CaseSensitive));    //count how many there are
        wc_inputText.remove(t_symbol_buffer, Qt::CaseSensitive);    //delete instances of this symbol from the workingcopy
    }

    //Calculate all probabilities
    std::for_each(index.begin(),index.end(),[=](Symbol& sym)
    {
        sym.setProb((double)sym.getCount()/(double)inputText.length());
    });

    //the list has to be sorted from highest to lowest probability
    qSort(index.begin(),index.end());

    //This function calculates and adds a code to each character:
    //  1.) split the vector into two vectors with equal probability
    //  2.) add a zero to the left and a one to the right vector
    //  3.) call this function recursivly for both vectors
    //  4.) merge all the vectors back together
    //
    //A more performant but less readable solution would be to not split the vector
    //and call the function with two additional iterators: void(SFList&, iterator, iterator)
    //
    //The function needs a recursion so there are only two feasable implemantions:
    //  1.) as a member function of SFCodec
    //  2.) as a lambda within SFCodec::updateIndex() as seen below
    //
    //The advantage of this lambda is that add_code() is only available where it's relevant -> within SFCodec::updateIndex().
    //This prevents unnecessarry (possibly harmfull) calls from outside.
    std::function<void(SFList&)> add_code = [&](SFList& vec_left)
    {
        if(vec_left.size() < 2)         //if there is only one element left we are done in this branch
            vec_left.begin()->setCode(QString("1"));
        else
        {

            SFList vec_right;     //add a vector to hold the right half of the tree

            SFList::split(vec_left, vec_right);

            //add zeros on the left and ones on the right
            if(vec_left.size())
                std::for_each(vec_left.begin(),vec_left.end(),[](Symbol& sym){sym.appendCode("0");});
            if(vec_right.size())
                std::for_each(vec_right.begin(), vec_right.end(), [](Symbol& sym){sym.appendCode("1");});

            //recursive call for each vector
            if(vec_left.size() > 1)
                add_code(vec_left);
            if(vec_right.size() > 1)
                add_code(vec_right);

            vec_left += vec_right; //and merge them all back together
        }

    };

    //note that the previous ~40 lines were a lambda function DECLARATION. Here we actually call that lambda function handing
    //over the index calculated before
    add_code(index);

}

void SFCodec::updateTable(QTableWidget* table)
{
    table->setColumnCount(4);
    table->setRowCount(index.size());

    QString data;
    int row = 0;
    std::for_each(index.begin(), index.end(), [&](Symbol entry)
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
    });
}

void SFCodec::updateStatus(QStatusBar* status)
{

}


QImage SFCodec::getTreeView(int width, int height)
{

    int treeWidth = width-50,                           //the tree should have 25px spacing on each side
        treeHeight = height-50,
        max_step_x = treeWidth/4, //max step length (i.e. the first step big step from the root)
        step_x = max_step_x,  //the current step length in x-direction
        step_y = 0,
        depth = 0,      //max depth of the tree
        length = 0;     //helper variable to calculate the depth

    QPoint p1(treeWidth/2,5),p2(0,0);           //Lines are drawn between two points. These are the two points...
    QImage image(width, height-1, QImage::Format_ARGB32); //new image with the right dimensions WORKAROUND: without the "-1" the label will expand upwards for some reason

    image.fill(QColor(255,255,255,255));    //filled in with white (NOTE: the format is ARGB so the first '255' is the alpha channel)
    if(index.isEmpty())                     //no input text -> no code -> no tree
        return image;

    QPainter painter(&image);   //Qt device that does all the painting

    painter.setPen(QPen(QColor(0,0,0)));    //paints in black
    painter.setRenderHint(QPainter::Antialiasing);  //activate antialiasing since performance isn't an issue

    //calculate depth by finding the symbol with the longest code (each digit in the code is one level of depth)
    std::for_each(index.begin(), index.end(),[&](Symbol sym)
    {
        length = sym.getCode().length();
        if(length > depth)
            depth = length;
    });

    //we have treeHeight many pixels and want to fill them with depth many levels. so the step length in y direction has to be this long:
    step_y = treeHeight/depth;





    std::function<void(SFList&, QPoint, int)> draw = [&](SFList list_left, QPoint p_begin, int level)
    {
        int pos = 0;
        SFList list_right;
        QPoint p_end;
        int t_step_x = step_x/pow(2, level);

        if(list_left.size())
        {
            QList<Symbol>::iterator iter = std::find_if(list_left.begin(), list_left.end(), [&](Symbol sym){return sym.getCode().at(level) == '1';});   //iterator to first symbol with a "1js" on this level's position

            pos = iter - list_left.begin();  //QList::mid() only takes int as parameter. Luckily pointer arithmetic is implemented with QList::iterators
            list_right = SFList(list_left.mid(pos));
            list_left = SFList(list_left.mid(0, pos));




            if(list_left.size())
            {
                p_end = p_begin + QPoint(-t_step_x,step_y);
                painter.drawLine(p_begin, p_end);

                if(level < 5)   //in higher levels there isn't enough space...
                {
                    painter.setPen(QPen(QColor(200,200,200)));  //paint numbers in grey
                    painter.drawText(p_begin + 0.25*(p_end - p_begin) + QPoint(-35,0), QString::number(list_left.sum(), 'f', 3).right(4));
                    painter.setPen(QPen(QColor(0,0,0)));  //back to black
                }

                if(list_left.size() > 1)
                    draw(list_left, p_end, level+1);
                else
                {
                    p_end = p_end + QPoint(-5,15);
                    painter.drawText(p_end, list_left.first().getSym());
                }

            }

            if(list_right.size())
            {
                p_end = p_begin + QPoint(t_step_x, step_y);
                painter.drawLine(p_begin, p_end);
                if(level < 5)   //in higher levels there isn't enough space...
                {
                    painter.setPen(QPen(QColor(200,200,200)));  //paint numbers in grey
                    painter.drawText(p_begin + 0.25*(p_end - p_begin) + QPoint(5,0), QString::number(list_right.sum(), 'f', 3).right(4));
                    painter.setPen(QPen(QColor(0,0,0)));  //back to black
                }
                if(list_right.size() > 1)
                    draw(list_right, p_end, level+1);
                else
                {
                    p_end = p_end + QPoint(-5,15);
                    painter.drawText(p_end, list_right.first().getSym());
                }
            }
        }

    };

    draw(index, p1, 0);
    painter.end();
    return image;
}
