#ifndef B_TREE_H
#define B_TREE_H

#define ORDER 6
#define MAXCHILDREN 6 
#define MAXKEYS 5
#define MINKEYS 2  
#define MINCHILDREN 3

typedef enum Bool{False,True}Bool;
typedef enum {FAMILY, USER, EXPENSE} NodeType;

typedef struct BTreeNode_tag{
    void* dataPointers[MAXKEYS];
    int numKeys;
    NodeType type;
    struct BTreeNode_tag* children[MAXCHILDREN];
}BTreeNode;

typedef struct BTree{
    NodeType type;
    BTreeNode* root;
}BTree;

void initializeBTree(BTree* tree,NodeType type);
Bool insertKey(BTree* tree,void *data);
Bool deleteKey(BTree* tree, int key);

//search
void* searchBTree(BTreeNode* root, int key);

//helpers
void deleteBTree(BTree* tree);
void printBFS(BTree* tree);
void printBTreeInOrder(BTreeNode* root);


#endif