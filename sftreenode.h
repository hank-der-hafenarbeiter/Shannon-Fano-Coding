#ifndef SFTREENODE_H
#define SFTREENODE_H

#include <QPainter>

#include <cmath>
#include <memory>
#include <tuple>

#include "sflist.h"

const int LABEL_LIMIT = 20; //Used in draw to prevend labels been drawn in branches that are to small


/**
 * @brief The SFTreeNode class is a simple binary tree implementation with special functionality for the Shannon Fano coding
 */
class SFTreeNode: public std::enable_shared_from_this<SFTreeNode>
{
public:
    SFTreeNode(SFList p_payload, std::shared_ptr<SFTreeNode> p_parent = 0, std::size_t p_distance_to_root = 0);

    void setRightChild(SFList p_payload);
    void setLeftChild(SFList p_payload);

    bool step();
    bool step_back();

    bool smallStep();

    std::size_t getShortestDistanceToLeaf(){return m_shortest_distance_to_leaf;}
    std::size_t getDistanceToRoot(){return m_distance_to_root;}

    double sumBranch();
    double balance();

    std::size_t depth();
    static QImage drawTree(std::shared_ptr<SFTreeNode> root, int width, int height);
private:

    void draw(QPainter& p_img, QPoint p_start, int p_distance_v, int p_distance_h);
    void killChildren();
    void setShortestDistanceToLeaf(size_t p_distance);
    void setDistanceToRoot(size_t p_distance);


    bool smallStep_helper_left();
    bool smallStep_helper_right();

    std::shared_ptr<SFTreeNode> m_parent;
    std::shared_ptr<SFTreeNode> m_left_child;
    std::shared_ptr<SFTreeNode> m_right_child;

    SFList m_payload;

    std::size_t m_distance_to_root;
    std::size_t m_shortest_distance_to_leaf;
};

#endif // SFTREENODE_H
