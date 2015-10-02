#include "sftreenode.h"

SFTreeNode::SFTreeNode(SFList p_payload, std::shared_ptr<SFTreeNode> p_parent, std::size_t p_distance_to_root):
    m_parent(p_parent),
    m_payload(p_payload),
    m_distance_to_root(p_distance_to_root),
    m_shortest_distance_to_leaf(0)
{
}

void SFTreeNode::setRightChild(SFList p_payload)
{
    m_right_child = std::make_shared<SFTreeNode>(p_payload, shared_from_this(), m_distance_to_root+1);
    setShortestDistanceToLeaf(1);
}

void SFTreeNode::setLeftChild(SFList p_payload)
{
    m_left_child = std::make_shared<SFTreeNode>(p_payload, shared_from_this(), m_distance_to_root+1);
    m_shortest_distance_to_leaf = 1;
    setShortestDistanceToLeaf(1);
}

bool SFTreeNode::step()
{
    bool result = false;

    if(m_payload.length() > 1)
    {
        SFList::iterator iter = SFList::split(m_payload.begin(), m_payload.end());
        int pos = iter - m_payload.begin();
        setLeftChild(m_payload.mid(0,pos));
        setRightChild(m_payload.mid(pos));
        m_payload.clear();
        result = true;
    }
    else
    {
        if(m_left_child)
            result = m_left_child->step() || result;
        if(m_right_child)
            result = m_right_child->step() ||result;
    }
    return result;
}

bool SFTreeNode::step_back()
{
    bool result = false;
    if(m_shortest_distance_to_leaf == 1)
    {
        if(m_left_child)
        {
            m_payload += m_left_child->m_payload;
            m_left_child.reset();
            result = true;
        }
        if(m_right_child)
        {
            m_payload += m_right_child->m_payload;
            m_right_child.reset();
            result = true;
        }
    }
    else
    {
        if(m_left_child)
            m_left_child->step_back();
        if(m_right_child)
            m_right_child->step_back();
    }

    if(!m_right_child && !m_left_child)
        setShortestDistanceToLeaf(0);
    return result;
}

bool SFTreeNode::smallStep()
{
    bool result = false;
    if(!m_left_child && !m_right_child) //first step of the process
    {                                   //all symbols in root
        setLeftChild(m_payload);        //spawn children and put all symbols into the left node
        setRightChild(SFList());        //balancing is done by smallStep_helper_left()
        m_payload.clear();
        result = true;
    }
    else                                                    //root was already split
    {
        if(m_left_child)                                    //continue with children
            result = m_left_child->smallStep_helper_left();
        if(m_right_child && !result)
            result = m_right_child->smallStep_helper_right();
    }
    return result;
}


bool SFTreeNode::smallStep_helper_left()
{
    bool result = false;
    if(m_payload.length() > 1)      //leaf of the tree in it's current form but not a leaf of the final tree (leafs only hold 1 symbol)
    {
        double balance = m_parent->balance();
        if(std::abs(balance) > std::abs(balance - (2*m_payload.last().getProb())))  //if the balance can be improved do so
        {
            m_parent->m_right_child->m_payload.push_front(m_payload.last());
            m_payload.pop_back();
            result = true;
        }
        else                                                                        //otherwise add children
        {
            setLeftChild(m_payload);
            setRightChild(SFList());
            m_payload.clear();
            result = true;
        }
    }
    else                                                                            //node without symbols => nodes within the tree
    {
        if(m_left_child)                                                            //nothing to do here so we move one to the two
            result = m_left_child->smallStep_helper_left();
        if(m_right_child && !result)
            result = m_right_child->smallStep_helper_right();                       //child nodes
    }
    return result;
}

bool SFTreeNode::smallStep_helper_right()
{
    bool result = false;
    if(m_payload.length() > 1)      //when we arrive on a right branch the balancing has already happened
    {                               //if there are more than one character in this node it can't be a leaf
        setLeftChild(m_payload);    //of the final tree so we need to add more children
        setRightChild(SFList());
        m_payload.clear();
        result = true;
    }
    else                                                        //node holds no symbols => note not a leaf
    {
        if(m_left_child)
            result = m_left_child->smallStep_helper_left();     //move on to child nodes
        if(m_right_child && !result)
            result = m_right_child->smallStep_helper_right();
    }
    return result;
}

QImage SFTreeNode::drawTree(std::shared_ptr<SFTreeNode> root, int width, int height)
{
    int treeWidth = width-10,                           //the tree should have 25px spacing on each side
        treeHeight = height-24,
        step_x = treeWidth/4, //max step length (i.e. the first step big step from the root)
        step_y = 0,
        depth = 0;      //max depth of the tree

    QPoint p1(treeWidth/2,5);           //Lines are drawn between two points. These are the two points...
    QImage image(width, height-1, QImage::Format_ARGB32); //new image with the right dimensions WORKAROUND: without the "-1" the label will expand upwards for some reason

    image.fill(QColor(255,255,255,255));    //filled in with white (NOTE: the format is ARGB so the first '255' is the alpha channel)

    if(root)                    //no input text -> no code -> no tree
    {
        QPainter painter(&image);   //Qt device that does all the painting

        painter.setPen(QPen(QColor(0,0,0)));    //paints in black
        painter.setRenderHint(QPainter::Antialiasing);  //activate antialiasing since performance isn't an issue

        //calculate depth by finding the symbol with the longest code (each digit in the code is one level of depth)
        depth = root->depth();
        if(!depth)
            depth = 1;
        //we have treeHeight many pixels and want to fill them with depth many levels. so the step length in y direction has to be this long:
        step_y = treeHeight/depth;
        root->draw(painter, p1, step_y, step_x);
    }
    return image;
}

void SFTreeNode::draw(QPainter& p_painter, QPoint p_start, int p_distance_v, int p_distance_h)
{
    QPoint p_end;
    if(m_left_child)
    {
        p_end = p_start + QPoint(-p_distance_h,p_distance_v);
        p_painter.drawLine(p_start, p_end);

        if(p_distance_h > LABEL_LIMIT)   //in higher levels there isn't enough space...
        {
            p_painter.setPen(QPen(QColor(200,200,200)));  //paint numbers in grey
            p_painter.drawText(p_start + 0.25*(p_end - p_start) + QPoint(-35,0),
                               QString::number(m_left_child->sumBranch(), 'f', 3).right(4));

            p_painter.setPen(QPen(QColor(0,0,0)));  //back to black
        }

        m_left_child->draw(p_painter, p_end, p_distance_v, p_distance_h/2);
    }

    if(m_right_child)
    {
        p_end = p_start + QPoint(p_distance_h, p_distance_v);
        p_painter.drawLine(p_start, p_end);

        if(p_distance_h > LABEL_LIMIT)   //in higher levels there isn't enough space...
        {
            p_painter.setPen(QPen(QColor(200,200,200)));  //paint numbers in grey
            p_painter.drawText(p_start + 0.25*(p_end - p_start) + QPoint(5,0),
                               QString::number(m_right_child->sumBranch(), 'f', 3).right(4));

            p_painter.setPen(QPen(QColor(0,0,0)));  //back to black
        }
        m_right_child->draw(p_painter, p_end, p_distance_v, p_distance_h/2);
    }
    else
    {
        QString str;
        for(Symbol sym:m_payload)
            str += sym.getSym();
        p_start += QPoint(-5*(str.length()/2),15);
        p_painter.drawText(p_start, str);

    }
}

void SFTreeNode::setShortestDistanceToLeaf(size_t p_distance)
{
    m_shortest_distance_to_leaf = p_distance;
    if(m_parent)
        m_parent->setShortestDistanceToLeaf(p_distance+1);
}

void SFTreeNode::setDistanceToRoot(size_t p_distance)
{
    m_distance_to_root = p_distance;
    if(m_left_child)
        m_left_child->setDistanceToRoot(p_distance+1);
    if(m_right_child)
        m_right_child->setDistanceToRoot(p_distance+1);
}

double SFTreeNode::sumBranch()
{
    double result = 0;
    for(Symbol sym:m_payload)
        result += sym.getProb();
    if(m_right_child)
        result += m_right_child->sumBranch();
    if(m_left_child)
        result += m_left_child->sumBranch();
    return result;
}

void SFTreeNode::killChildren()
{
    m_left_child.reset();
    m_right_child.reset();
}

double SFTreeNode::balance()
{
    double result = 0;
    if(m_left_child)
        result += m_left_child->sumBranch();
    if(m_right_child)
        result -= m_right_child->sumBranch();
    return result;
}

std::size_t SFTreeNode::depth()
{
    std::size_t depth_left = 0;
    std::size_t depth_right = 0;
    if(m_left_child)
    {
        depth_left = m_left_child->depth();
        depth_left++;
    }
    if(m_right_child)
    {
        depth_right = m_right_child->depth();
        depth_right++;
    }

    return (depth_left > depth_right)?(depth_left):(depth_right);
}
