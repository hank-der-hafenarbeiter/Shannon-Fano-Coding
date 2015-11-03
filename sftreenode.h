#ifndef SFTREENODE_H
#define SFTREENODE_H

#include <QPainter>

#include <cmath>
#include <memory>
#include <stack>
#include <tuple>

#include "sflist.h"


class SFTreeNode;


/**
 * @brief The SFTreeNode class is a simple binary tree implementation with special functionality for the Shannon Fano coding
 */
class SFTreeNode: public std::enable_shared_from_this<SFTreeNode>
{
private:
    //Types and constants
    const int LABEL_LIMIT = 20; //Used in draw to prevend labels been drawn in branches that are to small
    typedef std::tuple<std::shared_ptr<SFTreeNode>, int> StepInstruction ;     //represents a step in the construction of the tree. used in step_history. The following enums are used for the 2 ints
    typedef std::stack<StepInstruction> TreeHistoryStack;
    enum {SYMBOL_L_TO_R, NODE_SPLIT, BALANCED_NODE_SPLIT};

public:
    SFTreeNode(const SFList p_payload, std::shared_ptr<SFTreeNode> p_parent = 0, std::size_t p_distance_to_root = 0);

    ~SFTreeNode();

    void setRightChild(const SFList p_payload);
    void setLeftChild(const SFList p_payload);

    bool step();
    bool step_back();
    bool smallStep();

    std::size_t getShortestDistanceToLeaf() const {return m_shortest_distance_to_leaf;}
    std::size_t getDistanceToRoot() const {return m_distance_to_root;}

    double sumBranch() const;
    double balance() const;

    std::size_t depth();
    static QImage drawTree(std::shared_ptr<SFTreeNode> root, int width, int height);

private:

    void draw(QPainter& p_img, QPoint p_start, int p_distance_v, int p_distance_h) const;
    void killChildren();
    void setShortestDistanceToLeaf(size_t p_distance);
    void setDistanceToRoot(size_t p_distance);

    bool smallStep_helper_left();
    bool smallStep_helper_right();
    bool smallStepToBigStep();

    std::shared_ptr<SFTreeNode> m_parent;
    std::shared_ptr<SFTreeNode> m_left_child;
    std::shared_ptr<SFTreeNode> m_right_child;

    SFList m_payload;

    std::size_t m_distance_to_root;
    std::size_t m_shortest_distance_to_leaf;

    std::shared_ptr<TreeHistoryStack> m_step_history;


};

#endif // SFTREENODE_H
