#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
// NO_PATH if root is null, or UNEQUAL if the leaf depths differ.
static int leafDepth(Node * root) {
    if (root == nullptr) {
        return -2;
    }

    // A leaf: the only path from here to a leaf has length 0.
    if (root->left == nullptr && root->right == nullptr) {
        return 0;
    }

    int l = leafDepth(root->left);
    int r = leafDepth(root->right);

    if (l == -1 || r == -1) {
        return -1;
    }

    // Only one child exists, so every leaf lies down that single branch.
    if (l == -2) {
        return r + 1;
    }
    if (r == -2) {
        return l + 1;
    }

    // Both children exist: their leaf depths must match.
    return (l == r) ? l + 1 : -1;
}


bool equalPaths(Node * root)
{
  if (root == nullptr) {
        return true;
    }
    return leafDepth(root) != -1;
}

