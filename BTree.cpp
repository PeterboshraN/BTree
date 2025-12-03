#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;
template <typename T, int ORDER>
struct BTreeNode
{
    int n;                          // number of keys stored in the node (0 to order-1)
    bool is_leaf;                   // true if leaf node
    T *keys;                        // dynamic array of keys
    BTreeNode<T, ORDER> **children; // dynamic array of pointers to child nodes (pointer to array of pointers)

    BTreeNode(bool leaf = true)
    {
        n = 0;
        is_leaf = leaf;
        keys = new T[ORDER - 1]; //  because a node can have maximum order-1 keys
        children = new BTreeNode<T, ORDER> *[ORDER];
        for (int i = 0; i < ORDER; ++i)
            children[i] = nullptr; // set all child pointers to null
    }

    ~BTreeNode()
    {
        delete[] keys;
        delete[] children;
    }

    bool isFull()
    {
        return n == (ORDER - 1); // return true if the node has maximum keys
    }

    int findInsertIndex(const T &key)
    {
        int i = 0;
        while (i < n && !(key <= keys[i]))
            ++i;
        return i;
    }

    void insertKeyAtLeaf(int idx, const T &key)
    {
        for (int j = n - 1; j >= idx; --j)
            keys[j + 1] = keys[j]; // shifts then insert
        keys[idx] = key;
        ++n;
    }

    void insertKeyAt(int idx, const T &key)
    {
        for (int j = n - 1; j >= idx; --j)
            keys[j + 1] = keys[j];
        keys[idx] = key;
        ++n;
    }

    void shiftChildrenRightFrom(int idx)
    {
        for (int j = n; j >= idx; --j)
            children[j + 1] = children[j];
        children[idx] = nullptr;
    }

    void moveKeysTo(int srcStart, int count, BTreeNode *dest, int destStart)
    { // copy keys from a node to another
        for (int i = 0; i < count; ++i)
            dest->keys[destStart + i] = keys[srcStart + i];
        dest->n += count;
    }

    void moveChildrenTo(int srcStart, int count, BTreeNode *dest, int destStart)
    { // move child pointers from a node to another
        for (int i = 0; i < count; ++i)
        {
            dest->children[destStart + i] = children[srcStart + i];
            children[srcStart + i] = nullptr;
        }
    }

    void Print_node(BTreeNode<T, ORDER> *node)
    {
        for (int i = 0; i < node->n; i++)
        {
            cout << node->keys[i];
            if (i < node->n - 1)
                cout << ",";
        }
    }
    void Print_tree(BTreeNode<T, ORDER> *node, int level = 0)
    {
        if (node == nullptr)
            return;
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

template <typename T, int ORDER>
class BTree
{
private:
    BTreeNode<T, ORDER> *root;

    // Split a full child of a non-full parent
    void splitChild(BTreeNode<T, ORDER> *parent, int childIndex)
    {
        BTreeNode<T, ORDER> *fullChild = parent->children[childIndex];
        BTreeNode<T, ORDER> *newChild = new BTreeNode<T, ORDER>(fullChild->is_leaf);

        int midIndex = (ORDER - 1) / 2;

        // Move the right half of keys to the new child
        int numKeysToMove = ORDER - 1 - midIndex - 1;
        for (int i = 0; i < numKeysToMove; ++i)
        {
            newChild->keys[i] = fullChild->keys[midIndex + 1 + i];
        }
        newChild->n = numKeysToMove;

        // If not a leaf, move children as well
        if (!fullChild->is_leaf)
        {
            for (int i = 0; i <= numKeysToMove; ++i)
            {
                newChild->children[i] = fullChild->children[midIndex + 1 + i];
                fullChild->children[midIndex + 1 + i] = nullptr;
            }
        }

        // Update the full child's count
        fullChild->n = midIndex;

        // Shift parent's children to make room for new child
        parent->shiftChildrenRightFrom(childIndex + 1);
        parent->children[childIndex + 1] = newChild;

        // Insert the middle key into parent
        parent->insertKeyAt(childIndex, fullChild->keys[midIndex]);
    }

    // Insert into a non-full node
    void insertNonFull(BTreeNode<T, ORDER> *node, const T &key)
    {
        if (node->is_leaf)
        {
            // Insert directly into leaf
            int idx = node->findInsertIndex(key);
            node->insertKeyAtLeaf(idx, key);
        }
        else
        {
            // Find the child to insert into
            int idx = node->findInsertIndex(key);

            // If the child is full, split it first
            if (node->children[idx]->isFull())
            {
                splitChild(node, idx);

                // After split, determine which child to go to
                if (key > node->keys[idx])
                {
                    idx++;
                }
            }

            insertNonFull(node->children[idx], key);
        }
    }

public:
    // Constructor - initialize empty tree
    BTree()
    {
        root = nullptr;
    }

    // Insert a key into the tree
    void Insert(const T &key)
    {
        if (root == nullptr)
        {
            // Tree is empty - create first node
            root = new BTreeNode<T, ORDER>(true);
            root->keys[0] = key;
            root->n = 1;
        }
        else
        {
            // If root is full, split it
            if (root->isFull())
            {
                BTreeNode<T, ORDER> *newRoot = new BTreeNode<T, ORDER>(false);
                newRoot->children[0] = root;
                splitChild(newRoot, 0);
                root = newRoot;
            }

            insertNonFull(root, key);
        }
    }

    // Print the tree
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
};

int main()
{
    BTree<int, 3> t1;
    t1.Insert(1);
    t1.Insert(5);
    t1.Insert(0);
    t1.Insert(4);
    t1.Insert(3);
    t1.Insert(2);
    t1.Print();

    BTree<char, 5> t;
    t.Insert('G');
    t.Insert('I');
    t.Insert('B');
    t.Insert('J');
    t.Insert('C');
    t.Insert('A');
    t.Insert('K');
    t.Insert('E');
    t.Insert('D');
    t.Insert('S');
    t.Insert('T');
    t.Insert('R');
    t.Insert('L');
    t.Insert('F');
    t.Insert('H');
    t.Insert('M');
    t.Insert('N');
    t.Insert('P');
    t.Insert('Q');

    t.Print(); 


}