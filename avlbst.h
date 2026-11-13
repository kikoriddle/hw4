#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* n);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* n);
    // Fixes a node whose balance is -2 or +2 and returns the new subtree root.
    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* n);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
  const Key& k = new_item.first;
 
  if (this->root_ == NULL) {
    this->root_ = new AVLNode<Key, Value>(k, new_item.second, NULL);
    return;
  }
 
  // Ordinary BST descent to find the leaf position for k.
  AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
  AVLNode<Key, Value>* z = NULL;
  while (z == NULL) {
    if (k < curr->getKey()) {
      if (curr->getLeft() == NULL) {
        z = new AVLNode<Key, Value>(k, new_item.second, curr);
        curr->setLeft(z);
      }
      else {
        curr = curr->getLeft();
        }
      }
    else if (curr->getKey() < k) {
      if (curr->getRight() == NULL) {
        z = new AVLNode<Key, Value>(k, new_item.second, curr);
        curr->setRight(z);
      }
      else {
        curr = curr->getRight();
      }
    }
    else {
      // Key already present: overwrite the value, shape unchanged.
      curr->setValue(new_item.second);
      return;
    }
  }

    // Walk up from the new leaf. Each ancestor's subtree grew by one level,
    // so update its balance and stop as soon as the height stops changing.
    AVLNode<Key, Value>* child = z;
    AVLNode<Key, Value>* par = curr;
    while (par != NULL) {
        if (child == par->getLeft()) {
            par->updateBalance(-1);   // grew on the left: right - left decreases
        }
        else {
            par->updateBalance(1);    // grew on the right: right - left increases
        }
 
        int8_t b = par->getBalance();
        if (b == 0) {
            break;                    // subtree absorbed the new leaf; height unchanged
        }
        if (b == 2 || b == -2) {
            rebalance(par);           // a rotation restores the pre-insert height
            break;
        }
        child = par;                  // b == +1 or -1: height grew, keep going up
        par = par->getParent();
    }  
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
  AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (curr == NULL) {
        return;
    }
 
    // Two children: swap with the predecessor so curr has at most one child.
    // nodeSwap also swaps the balances, so they stay with their nodes.
    if (curr->getLeft() != NULL && curr->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(curr));
        this->nodeSwap(curr, pred);
    }
 
    // Splice out curr, promoting its single child (if any).
    AVLNode<Key, Value>* child = (curr->getLeft() != NULL) ? curr->getLeft() : curr->getRight();
    AVLNode<Key, Value>* par = curr->getParent();
    bool wasLeft = (par != NULL && par->getLeft() == curr);
 
    if (par == NULL) {
        this->root_ = child;
    }
    else if (wasLeft) {
        par->setLeft(child);
    }
    else {
        par->setRight(child);
    }
    if (child != NULL) {
        child->setParent(par);
    }
    delete curr;
 
    // Walk up from the removed spot. The side that lost a level increases its
    // balance in the sense of right - left. Stop once a subtree's height is unchanged.
    AVLNode<Key, Value>* n = par;
    bool leftShrunk = wasLeft;
    while (n != NULL) {
        AVLNode<Key, Value>* p = n->getParent();
        bool nIsLeft = (p != NULL && p->getLeft() == n);
 
        n->updateBalance(leftShrunk ? 1 : -1);
        int8_t b = n->getBalance();
 
        if (b == 2 || b == -2) {
            n = rebalance(n);         // new subtree root, p still its parent
            b = n->getBalance();
        }
        if (b != 0) {
            break;                    // height unchanged (balance is +1 or -1)
        }
        // height dropped by one: the parent may need work too
        n = p;
        leftShrunk = nIsLeft;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


/*
 * Single left rotation about n (n's right child r is promoted).
 * With balance = right - left:
 *   new b(n) = b(n) - 1 - max(b(r), 0)
 *   new b(r) = b(r) - 1 + min(new b(n), 0)
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* r = n->getRight();
    AVLNode<Key, Value>* inner = r->getLeft();
    AVLNode<Key, Value>* p = n->getParent();
 
    int bn = n->getBalance();
    int br = r->getBalance();
    int bnNew = bn - 1 - (br > 0 ? br : 0);
    int brNew = br - 1 + (bnNew < 0 ? bnNew : 0);
    n->setBalance(static_cast<int8_t>(bnNew));
    r->setBalance(static_cast<int8_t>(brNew));
 
    n->setRight(inner);
    if (inner != NULL) {
        inner->setParent(n);
    }
    r->setLeft(n);
    n->setParent(r);
 
    r->setParent(p);
    if (p == NULL) {
        this->root_ = r;
    }
    else if (p->getLeft() == n) {
        p->setLeft(r);
    }
    else {
        p->setRight(r);
    }
    return r;
}
 
/*
 * Single right rotation about n (n's left child l is promoted).
 * With balance = right - left:
 *   new b(n) = b(n) + 1 - min(b(l), 0)
 *   new b(l) = b(l) + 1 + max(new b(n), 0)
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* l = n->getLeft();
    AVLNode<Key, Value>* inner = l->getRight();
    AVLNode<Key, Value>* p = n->getParent();
 
    int bn = n->getBalance();
    int bl = l->getBalance();
    int bnNew = bn + 1 - (bl < 0 ? bl : 0);
    int blNew = bl + 1 + (bnNew > 0 ? bnNew : 0);
    n->setBalance(static_cast<int8_t>(bnNew));
    l->setBalance(static_cast<int8_t>(blNew));
 
    n->setLeft(inner);
    if (inner != NULL) {
        inner->setParent(n);
    }
    l->setRight(n);
    n->setParent(l);
 
    l->setParent(p);
    if (p == NULL) {
        this->root_ = l;
    }
    else if (p->getLeft() == n) {
        p->setLeft(l);
    }
    else {
        p->setRight(l);
    }
    return l;
}
 
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* n)
{
    if (n->getBalance() < 0) {
        // Left-heavy. If the left child leans right, rotate it first (left-right case).
        AVLNode<Key, Value>* l = n->getLeft();
        if (l->getBalance() > 0) {
            rotateLeft(l);
        }
        return rotateRight(n);
    }
    else {
        // Right-heavy. Mirror image of the case above.
        AVLNode<Key, Value>* r = n->getRight();
        if (r->getBalance() < 0) {
            rotateRight(r);
        }
        return rotateLeft(n);
    }
}

#endif
