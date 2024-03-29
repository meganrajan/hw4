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
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void rotateLeft(AVLNode<Key, Value>* x);
    void rotateRight(AVLNode<Key, Value>* x);
    void removeFix(AVLNode<Key, Value>* node, int diff);
    

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item) {
    //TODO

    if(this->root_ == nullptr) {
        // currentNode->setBalance(0);
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        this->root_ = newNode;
        return;
    }

    AVLNode<Key, Value>* currentNode = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parentNode = nullptr;
    
    // Find the correct position for the new node
    while (currentNode != nullptr) {
        parentNode = currentNode;
        if (new_item.first < currentNode->getKey()) {
            currentNode = currentNode->getLeft();
        } 
        else if (new_item.first > currentNode->getKey()) {
            currentNode = currentNode->getRight();
        } 
        else {
            // Key already exists, update the value
            currentNode->setValue(new_item.second);
            return; // Exit the function, no balance update needed
        }
    }

    // Create a new node and set its balance to 0, it will be a leaf node
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parentNode);
    newNode->setBalance(0);

    // Attach the new node to its parent
    if (parentNode == nullptr) {
        // The tree was empty, this new node is the root
        this->root_ = newNode;
    } 
    else if (new_item.first < parentNode->getKey()) {
        parentNode->setLeft(newNode);
        // parentNode->updateBalance(-1); // left heavy
        if(parentNode->getBalance() == -1 || parentNode->getBalance() == 1) {
            parentNode->setBalance(0);
        }
        else if(parentNode->getBalance() == 0) {
            parentNode->updateBalance(-1);
            insertFix(parentNode, newNode);
        }
    } 
    else {
        parentNode->setRight(newNode);
        // parentNode->updateBalance(1); // right heavy
        if(parentNode->getBalance() == -1 || parentNode->getBalance() == 1) {
            parentNode->setBalance(0);
        }
        else if(parentNode->getBalance() == 0) {
            parentNode->updateBalance(1);
            insertFix(parentNode, newNode);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n) {
    //TODO
    if (p == nullptr || p->getParent() == nullptr) {
        // root reached
        return;
    }
    
    AVLNode<Key, Value>* g = p->getParent();

    // ASSUME P IS LEFT CHILD OF G

    if(p == g->getLeft()) {
        g->updateBalance(-1);

        // case 1: 
        if(g->getBalance() == 0) {
            return;
        }

        // case 2:
        else if(g->getBalance() == -1) {
            insertFix(g, p);
        }

        // case 3
        else if(g->getBalance() == -2) {

            // PARENT IS ON LEFT OF GRANDPARENT BY ASSUMPTION
            
            // zig zig
            if(p->getLeft() == n) {
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }

            // zig zag
            if(p->getRight() == n) {
                rotateLeft(p);
                rotateRight(g);

                // case 3a
                if(n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                }
                // case 3b
                else if(n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                // case 3c
                else if(n->getBalance() == 1) {
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
            
        }

    }

    // __________________________________________________________________

        // ASSUME P IS RIGHT CHILD OF G
    if(p == g->getRight()) {
        g->updateBalance(1);

        // case 1: 
        if(g->getBalance() == 0) {
            return;
        }

        // case 2:
        else if(g->getBalance() == 1) {
            insertFix(g, p);
        }

        // case 3
        else if(g->getBalance() == 2) {

            // PARENT IS ON LEFT OF GRANDPARENT BY ASSUMPTION
            
            // zig zig
            if(p->getRight() == n) {
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }

            // zig zag
            if(p->getLeft() == n) {
                rotateRight(p);
                rotateLeft(g);

                // case 3a
                if(n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                }
                // case 3b
                else if(n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                // case 3c
                else if(n->getBalance() == -1) {
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            } 
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x) {
    //TODO
    AVLNode<Key, Value>* y = x->getRight(); // x's right child is y
    x->setRight(y->getLeft());
    if (y->getLeft() != nullptr) {
        y->getLeft()->setParent(x);
    }
    y->setParent(x->getParent());
    if (x->getParent() == nullptr) { // x was root
        this->root_ = y;
    } 
    else if (x == x->getParent()->getLeft()) { // x was left child
        x->getParent()->setLeft(y);
    } 
    else { // x was right child
        x->getParent()->setRight(y);
    }
    y->setLeft(x);
    x->setParent(y);


}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y) {
    //TODO
    AVLNode<Key, Value>* x = y->getLeft(); // y's left child is x
    // x->setRight(y);
    // y->setParent(x);

    y->setLeft(x->getRight());
    if (x->getRight() != nullptr) {
        x->getRight()->setParent(y);
    }
    x->setParent(y->getParent());
    if (y->getParent() == nullptr) { // y was root
        this->root_ = x;
    } 
    else if (y == y->getParent()->getLeft()) { // y was left child
        y->getParent()->setLeft(x);
    } 
    else { // y was right child
        y->getParent()->setRight(x);
    }
    x->setRight(y);
    y->setParent(x);

    // left side stays the same
    // at least 5 points that you need to change
    // 


}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    //TODO
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (nodeToRemove == nullptr) return; // node to remove doesn't exist, exit the function

    int diff = 0;


    // node has two children, find the in-order successor and swap positions
    if (nodeToRemove->getLeft() && nodeToRemove->getRight()) {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        nodeSwap(nodeToRemove, predecessor);
        // nodeToRemove = successor; // nodeToRemove will have at most one child
    }

    AVLNode<Key, Value>* parent = nodeToRemove->getParent();

    AVLNode<Key, Value>* child = nullptr;
    if (nodeToRemove->getLeft() != nullptr) {
        child = nodeToRemove->getLeft();
        diff = 1; // The right subtree is now taller
    } 
    else if (nodeToRemove->getRight() != nullptr) {
        child = nodeToRemove->getRight();
        diff = -1; // The left subtree is now taller
    }

    // If the node is the root
    if (nodeToRemove == this->root_) {
        this->root_ = child;
        if (child) {
            child->setParent(nullptr);
        }
    } 
    else {
        // If the node is not the root, adjust the parent's pointer
        if (nodeToRemove == parent->getLeft()) {
            parent->setLeft(child);

            //ADD TO DIFF
            diff = 1;
        } 
        else {
            parent->setRight(child);
            //SUBTRACT
            diff = -1;
        }
        if (child) {
            child->setParent(parent);
        }
    }

    delete nodeToRemove; // Delete the node

    // Fix the AVL tree balance if necessary
    removeFix(parent, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff) {
    //TODO
    if (n == nullptr) return; // If the node is null, return

    AVLNode<Key, Value>* p = n->getParent();
    int nextDiff;
    if(p != nullptr) {
        if(n == p->getLeft()) {
            nextDiff = 1;
        }
        else if (n == p->getRight()) {
            nextDiff = -1;
        }
    }

    // ASSUME DIFF = -1
    // case 1: b(n) + diff == -2
    if (diff == -1) {
        if(n->getBalance() + diff == -2) {
        AVLNode<Key, Value>* c = n->getLeft();
        // case 1a
        if(c->getBalance() == -1) { // zig-zig case {}
            rotateRight(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, nextDiff);
        } 

        // case 1b
        else if(c->getBalance() == 0) {
            rotateRight(n);
            n->setBalance(-1);
            c->setBalance(1);
        }

        // case 1c
        else if(c->getBalance() == 1) { // zig-zag case
            AVLNode<Key, Value>* g = c->getRight();
            rotateLeft(c);
            rotateRight(n);
            if(g->getBalance() == 1) {
                n->setBalance(0);
                c->setBalance(-1);
                g->setBalance(0);
            }

            else if(g->getBalance() == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }

            else if(g->getBalance() == -1) {
                n->setBalance(1);
                c->setBalance(0);
                g->setBalance(0);
            }

            removeFix(p, nextDiff);
        }
    }

        // case 2
        else if(n->getBalance() + diff == -1) {
            n->setBalance(-1);
        }
        // case 3
        else if(n->getBalance() + diff == 0) {
            n->setBalance(0);
            removeFix(p, nextDiff);
        }
    }
    
    else {
        // ASSUME DIFF = 1
        // case 1: b(n) + diff == 2
        if(n->getBalance() + diff == 2) {
            AVLNode<Key, Value>* c = n->getRight();
            // case 1a
            if(c->getBalance() == 1) { // zig-zig case {}
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, nextDiff);
            } 

            // case 1b
            else if(c->getBalance() == 0) {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }

            // case 1c
            else if(c->getBalance() == -1) { // zig-zag case
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if(g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }

                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                else if(g->getBalance() == 1) {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                removeFix(p, nextDiff);
            }
        }

        // case 2
        else if(n->getBalance() + diff == 1) {
            n->setBalance(1);
        }
        // case 3
        else if(n->getBalance() + diff == 0) {
            n->setBalance(0);
            removeFix(p, nextDiff);
        }
    }
    // __________________________________________________________

    

}



template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
