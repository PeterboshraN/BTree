#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;
template<typename T, int ORDER>
struct BTreeNode {
    int n;                              
    bool is_leaf;                      
    T *keys;                          
    BTreeNode<T,ORDER> **children;      

    BTreeNode(bool leaf = true) {       
        n = 0;
        is_leaf = leaf;
        keys = new T[ORDER - 1];
        children = new BTreeNode<T,ORDER>*[ORDER];
        for (int i = 0; i < ORDER; ++i) children[i] = nullptr;
    }

    ~BTreeNode() {                      
        delete[] keys;
        delete[] children;
    }

    bool isFull() {              
        return n == (ORDER - 1);
    }

    int findInsertIndex(const T &key) { 
        int i = 0;
        while (i < n && !(key <= keys[i])) ++i;
        return i;
    }

    void insertKeyAtLeaf(int idx, const T &key) { 
        for (int j = n - 1; j >= idx; --j) keys[j + 1] = keys[j];
        keys[idx] = key;
        ++n;
    }

    void insertKeyAt(int idx, const T &key) { 
        for (int j = n - 1; j >= idx; --j) keys[j + 1] = keys[j];
        keys[idx] = key;
        ++n;
    }

    void shiftChildrenRightFrom(int idx) { 
        for (int j = n; j >= idx; --j) children[j + 1] = children[j];
        children[idx] = nullptr;
    }

    void moveKeysTo(int srcStart, int count, BTreeNode *dest, int destStart) {
        for (int i = 0; i < count; ++i) dest->keys[destStart + i] = keys[srcStart + i];
        dest->n += count;
    }

    void moveChildrenTo(int srcStart, int count, BTreeNode *dest, int destStart) {
        for (int i = 0; i < count; ++i) {
            dest->children[destStart + i] = children[srcStart + i];
            children[srcStart + i] = nullptr;
        }
    }

    void Print_node(BTreeNode<T,ORDER>* node){
        for (int i = 0; i <= node->n; i++)
        {
            cout << node->keys[i];
            if (i < node->n - 1) cout << ",";
        }
    }
    void Print_tree(BTrrNode node, int level = 0)
    {
        if (node==nullptr) return ;
        for (int i = 0; i < level; i++)
        {
            cout << " ";
        }
        Print_node(node);
        cout << endl;
        if (!(node->is_leaf))
        {
            for (int i = 0; i <= node->n; i++)
            {
                Print_tree(node->children[i], level + 1);
            }
        }
    }
};

