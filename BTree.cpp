/*
    names               | IDS
    Peter Boshra        | 20230099
    Youssef Moris Kamal | 20230514
    Mohamed Ahmed Sayed | 20231133
    Emil Ehab Mosaad    | 20230067
*/

// Allowed libraries only
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

/*
    BTreeNode structure represents a node in a B-Tree.
    - n: number of keys currently stored
    - is_leaf: indicates whether the node is a leaf
    - keys: dynamic array storing keys
    - children: dynamic array storing children pointers
*/
template <typename T, int ORDER>
struct BTreeNode
{
    int n; // number of keys in the node
    bool is_leaf; // whether this node is a leaf
    T *keys; // dynamic array of keys
    BTreeNode<T, ORDER> **children; // array of child pointers

    // Constructor initializes the node
    BTreeNode(bool leaf = true)
    {
        n = 0;
        is_leaf = leaf;
        keys = new T[ORDER]; // allocate space for keys
        children = new BTreeNode<T, ORDER> *[ORDER + 1]; // children pointers

        // initialize children to nullptr
        for (int i = 0; i <= ORDER; i++)
        {
            children[i] = nullptr;
        }
    }

    // Destructor frees memory
    ~BTreeNode()
    {
        delete[] keys;
        delete[] children;
    }

    // Return true if node is full (contains ORDER-1 keys)
    bool isFull()
    {
        return n == (ORDER - 1);
    }

    // Find the index where a given key should be inserted
    int findInsertIndex(const T &key)
    {
        int i = 0;
        while (i < n && !(key <= keys[i]))
        {
            i++;
        }
        return i;
    }

    // Insert a key into a leaf node
    void insertKeyAtLeaf(int idx, const T &key)
    {
        for (int j = n - 1; j >= idx; j--)
        {
            keys[j + 1] = keys[j]; // shift keys
        }
        keys[idx] = key;
        n++;
    }

    // Insert key at a specific index (used by parent during split)
    void insertKeyAt(int idx, const T &key)
    {
        for (int j = n - 1; j >= idx; j--)
        {
            keys[j + 1] = keys[j]; // shift keys
        }
        keys[idx] = key;
        n++;
    }

    // Shift children one step to the right to make space
    void shiftChildrenRightFrom(int idx)
    {
        for (int j = n; j >= idx; j--)
        {
            children[j + 1] = children[j];
        }
        children[idx] = nullptr;
    }

    // Print keys of a single node
    void Print_node(BTreeNode<T, ORDER> *node)
    {
        for (int i = 0; i < node->n; i++)
        {
            cout << node->keys[i];
            if (i < node->n - 1)
            {
                cout << ",";
            }
        }
    }

    // Recursive function to print the entire B-Tree
    void Print_tree(BTreeNode<T, ORDER> *node, int level = 0)
    {
        if (node == nullptr)
        {
            return;
        }

        // indent based on level
        for (int i = 0; i < level; i++)
        {
            cout << "  ";
        }

        Print_node(node);
        cout << endl;

        // recurse through children
        if (!node->is_leaf)
        {
            for (int i = 0; i <= node->n; i++)
            {
                Print_tree(node->children[i], level + 1);
            }
        }
    }
};

/*
    BTree class implementing Insert and Print operations.
*/
template <typename T, int ORDER>
class BTree
{
private:
    BTreeNode<T, ORDER> *root; // pointer to root node

    // Split a full child node
    void splitChild(BTreeNode<T, ORDER> *parent, int childIndex)
    {
        BTreeNode<T, ORDER> *fullChild = parent->children[childIndex];
        BTreeNode<T, ORDER> *newChild = new BTreeNode<T, ORDER>(fullChild->is_leaf);

        int midIndex = ORDER / 2; // median key index

        // copy right half keys into new child
        int j = 0;
        for (int i = midIndex + 1; i < fullChild->n; i++)
        {
            newChild->keys[j] = fullChild->keys[i];
            j++;
        }
        newChild->n = j;

        // copy child pointers if internal node
        if (!fullChild->is_leaf)
        {
            j = 0;
            for (int i = midIndex + 1; i <= fullChild->n; i++)
            {
                newChild->children[j] = fullChild->children[i];
                fullChild->children[i] = nullptr;
                j++;
            }
        }

        // shrink full child
        fullChild->n = midIndex;

        // make space for new child in parent
        parent->shiftChildrenRightFrom(childIndex + 1);
        parent->children[childIndex + 1] = newChild;

        // move middle key to parent
        parent->insertKeyAt(childIndex, fullChild->keys[midIndex]);
    }

    // Insert key into node that is not full
    void insertNonFull(BTreeNode<T, ORDER> *node, const T &key)
    {
        if (node->is_leaf)
        {
            int idx = node->findInsertIndex(key);
            node->insertKeyAtLeaf(idx, key);
        }
        else
        {
            int idx = node->findInsertIndex(key);

            // recursively insert
            insertNonFull(node->children[idx], key);

            // after returning, check if child was overfull
            if (node->children[idx]->n == ORDER)
            {
                splitChild(node, idx);
            }
        }
    }

public:
    // constructor
    BTree()
    {
        root = nullptr;
    }

    // Insert a key in the tree
    void Insert(const T &key)
    {
        if (root == nullptr)
        {
            root = new BTreeNode<T, ORDER>(true);
            root->keys[0] = key;
            root->n = 1;
        }
        else
        {
            insertNonFull(root, key);

            // if root becomes full, split it
            if (root->n == ORDER)
            {
                BTreeNode<T, ORDER> *oldRoot = root;
                root = new BTreeNode<T, ORDER>(false);
                root->children[0] = oldRoot;
                root->n = 0;
                splitChild(root, 0);
            }
        }
    }

    // Print tree
    void Print()
    {
        if (root != nullptr)
        {
            root->Print_tree(root, 0);
        }
        else
        {
            cout << "Tree is empty" << endl;
        }
    }

    // destructor
    ~BTree()
    {
        deleteTree(root);
    }

private:
    // helper to delete tree recursively
    void deleteTree(BTreeNode<T, ORDER> *node)
    {
        if (node == nullptr)
        {
            return;
        }
        if (!node->is_leaf)
        {
            for (int i = 0; i <= node->n; i++)
            {
                deleteTree(node->children[i]);
            }
        }
        delete node;
    }
};

int main() {

    cout << "Test 1: Basic Inserts (no splits yet)\n";
    BTree<int, 3> t1;
    t1.Insert(1);
    t1.Insert(5);
    t1.Insert(0);
    t1.Insert(4);
    t1.Insert(3);
    t1.Insert(2);
    t1.Print();


    cout << "\nTest 2: Insert Until Root Splits\n";
    BTree<int, 3> t2;
    t2.Insert(10);
    t2.Insert(20);
    t2.Insert(5);   // root should split here
    t2.Insert(6);
    t2.Insert(12);
    t2.Print();


    cout << "\nTest 3: Ascending Inserts\n";
    BTree<int, 3> t3;
    for (int i = 0; i < 15; i++)
        t3.Insert(i);
    t3.Print();


    cout << "\nTest 4: Descending Inserts\n";
    BTree<int, 3> t4;
    for (int i = 15; i >= 0; i--)
        t4.Insert(i);
    t4.Print();


    cout << "\nTest 5: Random Inserts\n";
    BTree<int, 3> t5;
    int arr1[] = {15, 3, 8, 22, 5, 12, 30, 7, 2, 14, 9, 28, 1, 6};
    for (int x : arr1)
        t5.Insert(x);
    t5.Print();


    cout << "\nTest 6: Insert Duplicates\n";
    BTree<int, 3> t6;
    t6.Insert(50);
    t6.Insert(50);
    t6.Insert(50);
    t6.Insert(30);
    t6.Insert(30);
    t6.Insert(40);
    t6.Print();


    cout << "\nTest 7: Insert Into Deep Tree\n";
    BTree<int, 3> t7;
    int arr2[] = {40, 10, 20, 30, 50, 60, 70, 5, 15, 25, 35, 45, 55, 65, 75};
    for (int x : arr2)
        t7.Insert(x);
    t7.Print();


    cout << "\nTest 8: Stress Test\n";
    BTree<int, 3> t8;
    for (int i = 0; i < 50; i++)
        t8.Insert(i * 2);
    t8.Print();

    return 0;
}
