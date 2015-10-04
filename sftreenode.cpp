#include "sftreenode.h"

SFTreeNode::SFTreeNode(SFList p_payload, std::shared_ptr<SFTreeNode> p_parent, std::size_t p_distance_to_root):
    m_parent(p_parent),
    m_payload(p_payload),
    m_distance_to_root(p_distance_to_root),
    m_shortest_distance_to_leaf(0)
{
}

/**
 * @brief SFTreeNode::setRightChild creates a right child with the given payload setting m_shortest_distance_to_leaf to 1
 * @param p_payload is the payload the child will be constructed with
 */
void SFTreeNode::setRightChild(const SFList p_payload)
{
    m_right_child = std::make_shared<SFTreeNode>(p_payload, shared_from_this(), m_distance_to_root+1);
    setShortestDistanceToLeaf(1);
}

/**
 * @brief SFTreeNode::setLeftChild creates a left child with the given payload setting m_shortest_distance_to_leaf to 1
 * @param p_payload is the payload the child will be constructed with
 */
void SFTreeNode::setLeftChild(const SFList p_payload)
{
    m_left_child = std::make_shared<SFTreeNode>(p_payload, shared_from_this(), m_distance_to_root+1);
    m_shortest_distance_to_leaf = 1;
    setShortestDistanceToLeaf(1);
}

/**
 * @brief SFTreeNode::step takes one (big) step in constructing the binary tree corresponding to the shannon fano coding
 * @return true if the tree was modified in this node or one of it's children. false elswise
 * SFTreeNode::step() takes one (big) step in constructing the binary tree corresponding to the shannon fano coding.
 * In contrast to SFTreeNode::smallStep() it does not show how the tree is balanced. Splits are made off screen
 */
bool SFTreeNode::step()
{
    bool result = false;

    if(m_payload.length() > 1)  //Node contains more than one Symbol it will be an inner node in the final tree therefore
    {
        SFList::iterator iter = SFList::split(m_payload.begin(), m_payload.end());  //the payload needs to be split into two
        int pos = iter - m_payload.begin();
        setLeftChild(m_payload.mid(0,pos));                                         //distributed to the two child nodes
        setRightChild(m_payload.mid(pos));
        m_payload.clear();                                                          //SFList::mid() constructs a copy
        result = true;                                                              //so m_payload needs to be cleared
    }
    else
    {
        if(m_left_child)                                            //if nothing was changed (result = false)
            result =result || m_left_child->step();                 //proceed with left child
        if(m_right_child)                                           //if still nothing was changed
            result = result || m_right_child->step();               //proceed with right child
    }                                                               //note that m_right/left_child->step() is only calle if result = false
    return result;
}

/**
 * @brief SFTreeNode::step_back undos the last call of SFTreeNode::step()
 * @return true if the tree was modified in this node or one of it's children.false elswise
 */
bool SFTreeNode::step_back()
{
    bool result = false;
    if(m_shortest_distance_to_leaf == 1)                //if this is the last node before a leaf
    {
        if(m_left_child)
        {
            m_payload += m_left_child->m_payload;       //put the payload of the child into this node
            m_left_child.reset();                       //and delete the child
            result = true;
        }
        if(m_right_child)                               //do the same for the other child
        {
            m_payload += m_right_child->m_payload;
            m_right_child.reset();
            result = true;
        }
    }
    else
    {
        if(m_left_child)                                    //if the children aren't leafs call their step_back()
            result = result || m_left_child->step_back();
        if(m_right_child)
            result = result || m_right_child->step_back();
    }

    if(!m_right_child && !m_left_child)                     //if the current node has no children left it becomes a leaf
        setShortestDistanceToLeaf(0);
    return result;
}

/**
 * @brief SFTreeNode::smallStep takes one (small) step in constructing the binary tree corresponding to the shannon fano coding
 * @return true if the tree was modified in this node or one of it's children. false elswise
 * SFTreeNode::smallStep takes one (small) step in constructing the binary tree corresponding to the shannon fano coding
 * In contrast to SFTreeNode::step() it shows how the splitting of a node with multiple symbols is done
 */
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

/**
 * @brief SFTreeNode::smallStep_helper_left is called if this node should make a (small) step and it is the left child of its parent node
 * @return  true if the tree was modified in this node or one of it's children. false elswise
 */
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

/**
 * @brief SFTreeNode::smallStep_helper_right is called if this node should make a (small) step and it is the right child of its parent node
 * @return true if the tree was modified in this node or one of it's children. false elswise
 */
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

/**
 * @brief SFTreeNode::drawTree creates a QImage depicting the tree starting at p_root
 * @param p_root is the starting point of the tree that is to be drawn
 * @param p_width is the width the resulting QImage should have
 * @param p_height is the height the resulting QImage should have
 * @return QImage depicting the tree starting at the given root
 */
QImage SFTreeNode::drawTree(std::shared_ptr<SFTreeNode> p_root, int p_width, int p_height)
{
    int treeWidth = p_width-10,
        treeHeight = p_height-24,
        step_x = treeWidth/4,
        step_y = 0,
        depth = 0;

    QPoint p1(treeWidth/2,5);
    QImage image(p_width, p_height-1, QImage::Format_ARGB32); //new image with the right dimensions WORKAROUND: without the "-1" the label will expand upwards for some reason

    image.fill(QColor(255,255,255,255));    //filled in with white (NOTE: the format is ARGB so the first '255' is the alpha channel)

    if(p_root)                    //check if root is a valid pointer
    {
        QPainter painter(&image);

        painter.setPen(QPen(QColor(0,0,0)));
        painter.setRenderHint(QPainter::Antialiasing);

        depth = p_root->depth();
        if(!depth)
            depth = 1;

        step_y = treeHeight/depth;
        p_root->draw(painter, p1, step_y, step_x);
    }
    return image;
}

/**
 * @brief SFTreeNode::draw draws a tree consisting of this node and all it's children starting at the given starting point
 * @param p_painter QPainter in which the tree is drawn
 * @param p_start QPoint containing the position where this node should be drawn
 * @param p_distance_v vertical distance to children
 * @param p_distance_h horizontal distance to children
 */
void SFTreeNode::draw(QPainter& p_painter, QPoint p_start, int p_distance_v, int p_distance_h)
{
    QPoint p_end;
    if(m_left_child)
    {
        p_end = p_start + QPoint(-p_distance_h,p_distance_v);
        p_painter.drawLine(p_start, p_end);

        if(p_distance_h > LABEL_LIMIT)   //check if there is enough space to display the probability of the current branch
        {
            p_painter.setPen(QPen(QColor(200,200,200)));

            p_painter.drawText(p_start + 0.25*(p_end - p_start) + QPoint(-35,0),
                               QString::number(m_left_child->sumBranch(), 'f', 3).right(4));

            p_painter.setPen(QPen(QColor(0,0,0)));
        }

        m_left_child->draw(p_painter, p_end, p_distance_v, p_distance_h/2);
    }

    if(m_right_child)
    {
        p_end = p_start + QPoint(p_distance_h, p_distance_v);
        p_painter.drawLine(p_start, p_end);

        if(p_distance_h > LABEL_LIMIT)
        {
            p_painter.setPen(QPen(QColor(200,200,200)));
            p_painter.drawText(p_start + 0.25*(p_end - p_start) + QPoint(5,0),
                               QString::number(m_right_child->sumBranch(), 'f', 3).right(4));

            p_painter.setPen(QPen(QColor(0,0,0)));
        }
        m_right_child->draw(p_painter, p_end, p_distance_v, p_distance_h/2);
    }
    else                                    //no children => current node is a leaf => draw it's symbol
    {
        QString str;
        for(Symbol sym:m_payload)
            str += sym.getSym();
        p_start += QPoint(-5*(str.length()/2),15);
        p_painter.drawText(p_start, str);

    }
}

/**
 * @brief SFTreeNode::setShortestDistanceToLeaf sets SFTreeNode::m_shortest_distance_to_leaf of this node and it's parent if it exists
 * @param p_distance the distance to the closest leaf
 */
void SFTreeNode::setShortestDistanceToLeaf(size_t p_distance)
{
    m_shortest_distance_to_leaf = p_distance;
    if(m_parent)
        m_parent->setShortestDistanceToLeaf(p_distance+1);
}

/**
 * @brief SFTreeNode::setDistanceToRoot sets SFTreeNode::m_distance_to_root of this node and it's children if they exist
 * @param p_distance the distance to the root
 */
void SFTreeNode::setDistanceToRoot(size_t p_distance)
{
    m_distance_to_root = p_distance;
    if(m_left_child)
        m_left_child->setDistanceToRoot(p_distance+1);
    if(m_right_child)
        m_right_child->setDistanceToRoot(p_distance+1);
}

/**
 * @brief SFTreeNode::sumBranch return the sum of all symbols this node and all its children contain
 * @return sum of all symbols this node and all its children contain
 */
double SFTreeNode::sumBranch() const
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

/**
 * @brief SFTreeNode::killChildren destroys all children of this node
 */
void SFTreeNode::killChildren()
{
    if(m_left_child)
        m_left_child.reset();
    if(m_right_child)
        m_right_child.reset();
}

/**
 * @brief SFTreeNode::balance gives the difference between the sum of the right child tree substracted from the sum of the left child tree
 * @return difference between the two child trees
 */
double SFTreeNode::balance() const
{
    double result = 0;
    if(m_left_child)
        result += m_left_child->sumBranch();
    if(m_right_child)
        result -= m_right_child->sumBranch();
    return result;
}

/**
 * @brief SFTreeNode::depth returns the distance from this node to the farthest leaf
 * @return distance from this note to the farthest leaf
 */
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

    return (depth_left > depth_right)?(depth_left):(depth_right);   //return the bigge of the wo values
}
