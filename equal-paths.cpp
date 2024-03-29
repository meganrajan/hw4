#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
    // Add your code below
    if(root == nullptr) {
        return true;
    }

    int leafDepth = -1;
    return checkEqualPaths(root, 1, leafDepth);
    
}

bool checkEqualPaths(Node* root, int depth, int &leafDepth) {

    if(root == NULL) {
        return true;
    }

    if(root->left == NULL && root->right == NULL) {
        if(leafDepth == -1) {
            leafDepth = depth;
            return true;
        }

        return depth == leafDepth;
    }

    bool left = checkEqualPaths(root->left, depth + 1, leafDepth);
    bool right = checkEqualPaths(root->right, depth + 1, leafDepth);

    return left && right;
}

