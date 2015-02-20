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
    //and call the function with two additional iterators: void(SFVector&, iterator, iterator)
    //
    //The function needs a recursion so there are only two feasable implemantions:
    //  1.) as a member function of SFCodec
    //  2.) as a lambda within SFCodec::updateIndex() as seen below
    //
    //The advantage of this lambda is that add_code() is only available where it's relevant -> within SFCodec::updateIndex().
    //This prevents unnecessarry (possibly harmfull) calls from outside.
    std::function<void(SFVector&)> add_code = [&](SFVector& vec_left)
    {
        if(vec_left.size() < 2)         //if there is only one element left we are done in this branch
        {
            vec_left.begin()->setCode(QString("1"));
            return;
        }

        SFVector vec_right;     //add a vector to hold the right half of the tree

        double diff1 = 1, diff2 = 0; //the difference between the sum of probabilities during the previous and current iteration

        do
        {
            vec_right.prepend(vec_left.last());
            vec_left.pop_back();
            diff2 = diff1;
            diff1 = fabs(vec_left.sum() - vec_right.sum());
        }while((diff1-diff2) < 0); //compare two diffs to check if the iteration improved or worsened the result

        //the loop continues until it worsened the difference once so we have to roll back once
        vec_left.append(vec_right.first());
        vec_right.pop_front();

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
    qDebug() << width << " " << height;
    int treeWidth = width-50,                           //the tree should have 25px spacing on each side
        treeHeight = height-50,
        max_step_x = treeWidth/4, //max step length (i.e. the first step big step from the root)
        step_x = max_step_x,  //the current step length in x-direction
        step_y = 0,
        depth = 0,      //max depth of the tree
        length = 0;     //helper variable to calculate the depth

    QPoint p1(treeWidth/2,5),p2(0,0);           //Lines are drawn between two points. These are the two points...
    QImage image(width, height, QImage::Format_ARGB32); //new image with the right dimensions

    image.fill(QColor(255,255,255,255));    //filled in with white (NOTE: the format is ARGB so the first '255' is the alpha channel)
    if(index.isEmpty())                     //no input text -> no code -> no tree
        return image;

    QPainter painter(&image);   //Qt device that does all the painting

    painter.setPen(QPen(QColor(0,0,0)));    //paints in black
    painter.setBrush(QBrush(QColor(Qt::color0), Qt::NoBrush));
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


    //paint the path for each symbol
    //note that we always start from the root so some lines are painted multiple times
    //this could be prevented by a recursive call which would be less readable and
    //(probably) not significantly faster.
    std::for_each(index.begin(), index.end(),[&](Symbol sym)
    {
        p1 = QPoint(treeWidth/2,5);  //reset to starting position
        step_x = max_step_x;        //and step length
        QString temp = sym.getCode();
        do{
           if(temp.at(0) == '1')
                p2 = p1 + QPoint(step_x,step_y);
            else
                p2 = p1 + QPoint(-step_x,step_y);
            step_x = step_x/2;

            painter.drawLine(p1,p2);
            p1 = p2;

            temp.remove(0,1);
        }while(temp.size());
        p1 = p1 + QPoint(-5,15);
        painter.drawText(p1, sym.getSym());
    });
    painter.end();
    return image;
}
