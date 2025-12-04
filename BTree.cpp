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
        keys = new T[ORDER]; //  will cause overflow but it will be detected 
        children = new BTreeNode<T, ORDER> *[ORDER+1];
        for (int i = 0; i <= ORDER; ++i)
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
            cout << "  ";
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

    // Split an overfull child (has ORDER keys instead of ORDER-1)
    void splitChild(BTreeNode<T, ORDER> *parent, int childIndex)
    {
        BTreeNode<T, ORDER> *fullChild = parent->children[childIndex];
        BTreeNode<T, ORDER> *newChild = new BTreeNode<T, ORDER>(fullChild->is_leaf);

        // With ORDER keys (overfull), split at position ORDER/2
        int midIndex = ORDER / 2;
        
        // Copy upper half to new child
        int j = 0;
        for (int i = midIndex + 1; i < fullChild->n; ++i)
        {
            newChild->keys[j++] = fullChild->keys[i];
        }
        newChild->n = j;

        // Copy children if internal node
        if (!fullChild->is_leaf)
        {
            j = 0;
            for (int i = midIndex + 1; i <= fullChild->n; ++i)
            {
                newChild->children[j++] = fullChild->children[i];
                fullChild->children[i] = nullptr;
            }
        }

        // Update fullChild's count (keeps left half)
        fullChild->n = midIndex;

        // Make room in parent for new child
        parent->shiftChildrenRightFrom(childIndex + 1);
        parent->children[childIndex + 1] = newChild;

        // Move middle key to parent
        parent->insertKeyAt(childIndex, fullChild->keys[midIndex]);
    }

    // Insert into a node (may cause it to become overfull)
    void insertNonFull(BTreeNode<T, ORDER> *node, const T &key)
    {
        if (node->is_leaf)
        {
            // Insert directly into leaf (may become overfull)
            int idx = node->findInsertIndex(key);
            node->insertKeyAtLeaf(idx, key);
        }
        else
        {
            // Find the child to insert into
            int idx = node->findInsertIndex(key);
            
            // Recurse to child
            insertNonFull(node->children[idx], key);
            
            // After recursion, if child is overfull, split it
            if (node->children[idx]->n == ORDER)
            {
                splitChild(node, idx);
            }
        }
    }

public:
    BTree()
    {
        root = nullptr;
    }

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
            // Insert into root (may become overfull)
            insertNonFull(root, key);
            
            // If root became overfull, split it
            if (root->n == ORDER)
            {
                BTreeNode<T, ORDER> *oldRoot = root;
                root = new BTreeNode<T, ORDER>(false);  // New root
                root->children[0] = oldRoot;
                root->n = 0;  // Start with 0 keys
                splitChild(root, 0);  // Split the old root
            }
        }
    }

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

    ~BTree()
{
    deleteTree(root);
}

private:
    void deleteTree(BTreeNode<T, ORDER> *node)
    {
        if (node == nullptr) return;
        
        if (! node->is_leaf)
        {
            for (int i = 0; i <= node->n; ++i)
            {
                deleteTree(node->children[i]);
            }
        }
        delete node;
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

    cout<<endl;
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