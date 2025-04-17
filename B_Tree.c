#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "B_Tree.h"
#include "queue.h"
#include "family.h"
#include "user.h"
#include "expense.h"

//function declarations

void initializeBTree(BTree* tree,NodeType type);
// BTreeNode* createBTreeNode();
int getIdOfNode(void* data,NodeType type);

//Insert Functions

//Inserting helper
Bool traverseAndInsertRec(BTreeNode* root,void* dataPtr ,BTreeNode** ptr1_db, BTreeNode** ptr2_db,void** newMedianDataPtr_db);
void insertInLeafNode(void* arr[],void* dataPtr, int loc);
Bool splitLeafNode(BTreeNode* filledNode,void* dataPtr,int insertLoc,BTreeNode** ptr1_db,BTreeNode** ptr2_db, void** medianDataPtr);
void insertMedianAndChildPtrsInParent(BTreeNode* parent,int childIndex,void* medianDataPtrFromChild,BTreeNode* ptr1_child, BTreeNode* ptr2_child);
Bool insertChildPtrsAndMedianAndSplitParent(BTreeNode* parent,void* medianDataPtrFromChild,int childIndex,BTreeNode* ptr1_child,BTreeNode* ptr2_child,BTreeNode** ptr1_split_db,BTreeNode** ptr2_split_db,void** newMedianDataPtr);

//Deleting helper
Bool removeKey(BTreeNode* node,int key);
Bool removeFromNonLeaf(BTreeNode* node,int idx,int key);
void* getPredecessor(BTreeNode* node, int idx);
void* getSuccessor(BTreeNode* node,int idx);
void borrowFromPrev(BTreeNode* node, int idx);
void borrowFromNext(BTreeNode* node, int idx);
void mergeNodes(BTreeNode* node, int idx);
void fillChild(BTreeNode* node, int idx);

void copyData(void* node1, void* node2, NodeType type);
void deleteDataPtr(void* dataPtr,NodeType type);

//Delete function Helper

//helper functions
void deleteNodeRec(BTreeNode* root);
void deleteBTree(BTree* tree);
void printBFS(BTree* tree);

//function definitions
void initializeBTree(BTree* tree,NodeType type){
    tree->type=type;
    tree->root=NULL;
}

BTreeNode* createBTreeNode(NodeType type){
    BTreeNode* ret_node=NULL;
    ret_node= (BTreeNode*) malloc(sizeof(BTreeNode));
    if(ret_node!=NULL){
        ret_node->numKeys=0;//num keys are 0
        for(int i=0;i<MAXKEYS;i++){
            ret_node->dataPointers[i]=NULL;
            ret_node->children[i]=NULL;
            ret_node->type=type;
        }
        ret_node->children[MAXCHILDREN-1]=NULL;
    }
    return ret_node;
}

int getIdOfNode(void* data,NodeType type){
    int key;
    switch (type) {
        case FAMILY:
            key = ((Family*)data)->familyId;
            break;
        case USER:
            key = ((User*)data)->userId;
            break;
        case EXPENSE:
            key = ((Expense*)data)->expenseId;
            break;
    }
    return key;
}


//?Insert related functions start
Bool insertKey(BTree* tree,void *dataPtr){
    int id=getIdOfNode(dataPtr,tree->type);
    Bool inserted=False;
    if(tree==NULL || id==0){
        inserted=False;
    }    
    else if(tree->root==NULL){
        // printf("\nRoot is null creating root\n");
        tree->root=createBTreeNode(tree->type);
        if(tree->root!=NULL){
            tree->root->dataPointers[0]=dataPtr;
            tree->root->numKeys=1;
            inserted=True;
        }
    }
    else{
        BTreeNode* root=tree->root;
        //from the recursion we may get a median and two pointers to handle it in the current parent node
        BTreeNode* ptr1=NULL;
        BTreeNode* ptr2=NULL;
        void* newMedianDataPointer=NULL;
        inserted=traverseAndInsertRec(root,dataPtr,&ptr1,&ptr2,&newMedianDataPointer);
        if(ptr1!=NULL && ptr2!=NULL && newMedianDataPointer!=NULL && inserted){
            //means we have two subtrees, we have to create a root with the given median and attach them here
            BTreeNode* newRoot=createBTreeNode(tree->type);
            if(newRoot==NULL){
                inserted=False;
                // printf("\nFailed to create new Root\n");
            }
            else{
                // printf("\nCreated new root\n");
                newRoot->dataPointers[0]=newMedianDataPointer;
                newRoot->children[0]=ptr1;
                newRoot->children[1]=ptr2;
                newRoot->numKeys=1;
                tree->root=newRoot;
                inserted=True;
            }
        }
    }
    return inserted;
}

//the bool indicates if the new data node is created or not (if not enough space), or if enough space in the node, the bool indicates inserted successfully or not
Bool traverseAndInsertRec(BTreeNode* root,void* dataPtr ,BTreeNode** ptr1_db, BTreeNode** ptr2_db,void** newMedianDataPtr_db){
    int id=getIdOfNode(dataPtr,root->type);
    //here root is not null initially, this function is called only if root is not null
    //in this node, find the first id greater than the id about to be inserted
    Bool inserted=False;
    Bool goToChild=False;// do we have to go to child node to insert?
    if(root->children[0]!=NULL){
        goToChild=True;
    }
    //searching for id
    int numKeysInNode=root->numKeys;
    Bool nextHigherKeyFound=False;
    int insertLoc=0;
    while(insertLoc<numKeysInNode && !nextHigherKeyFound){
        if(id<getIdOfNode(root->dataPointers[insertLoc],root->type)){
            nextHigherKeyFound=True;
        }
        else{
            insertLoc++;
        }
    }
    if(!goToChild){
        //then insert here if possible or split
        //this is the leaf node
        if(root->numKeys<MAXKEYS){
            //insert in this only
            insertInLeafNode(root->dataPointers,dataPtr,insertLoc);
            //increment numkeys
            root->numKeys=root->numKeys+1;
            inserted=True;
        }
        else{
            //!make a split block function which takes in current block and new value/id or median from child and returns ptr1 adn ptr2
            // printf("\nHave to split the node because its full\n");
            int insertLocOfKey=insertLoc;
            BTreeNode* ptr1=NULL;
            BTreeNode* ptr2=NULL;
            void* newMedianDataPtrFromSplit=0;
            Bool splitted=splitLeafNode(root,dataPtr,insertLocOfKey,&ptr1,&ptr2,&newMedianDataPtrFromSplit);
            if(!splitted){
                // printf("\nFailed to split\n");
                inserted=False;
            }
            else{
                *ptr1_db=ptr1;
                *ptr2_db=ptr2;
                *newMedianDataPtr_db=newMedianDataPtrFromSplit;
                inserted=True;
            }
        }
    }
    else{
        int childIndex;//we have to know at which index the child pointer is present
        childIndex=insertLoc;
        BTreeNode* childNode=root->children[childIndex];
        BTreeNode* ptr1_child=NULL,*ptr2_child=NULL;
        void* medianDataPtrFromChild=NULL;
        // printf("\nGoing to child\n");
        inserted=traverseAndInsertRec(childNode,dataPtr,&ptr1_child,&ptr2_child,&medianDataPtrFromChild);
        //now if the median and two pointers from the child are not null handle them here
        //try inserting the median into current node, or split the node into two blocks and send them to parent, i.e update ptr1_db and ptr2_db
        if(ptr1_child!=NULL && ptr2_child!=NULL && medianDataPtrFromChild!=NULL && inserted){
            //now we have to insert this median into parent node
            // printf("\nInserting median in parent\n");
            if(root->numKeys<MAXKEYS){
                //we can insert
                insertMedianAndChildPtrsInParent(root,childIndex,medianDataPtrFromChild,ptr1_child,ptr2_child);
                inserted=True;
            }
            else{
                // printf("\nHave to split parent Node because its full\n");
                //split parent(current) node
                //the two pointers of node ptr1_db ptr2_db will be updated in this function after splitting the node
                inserted=insertChildPtrsAndMedianAndSplitParent(root,medianDataPtrFromChild,childIndex,ptr1_child,ptr2_child,ptr1_db,ptr2_db,newMedianDataPtr_db);
                if(!inserted){
                    // printf("\nFailed to split parent node, Data lost\n");
                }
            }
        }
    }
    return inserted;
}

Bool insertChildPtrsAndMedianAndSplitParent(BTreeNode* parent,void* medianDataPtrFromChild,int childIndex,BTreeNode* ptr1_child,BTreeNode* ptr2_child,BTreeNode** ptr1_split_db,BTreeNode** ptr2_split_db,void** newMedianDataPtr){
    Bool splitted=True;
    BTreeNode* parent2=createBTreeNode(parent->type);
    if(parent2==NULL){
        splitted=False;
    }
    else{
        void* pooledDataPtrs[MAXKEYS+1];
        BTreeNode* pooledChildren[MAXCHILDREN+1];
        for(int i=0;i<MAXKEYS;i++){
            pooledDataPtrs[i]=parent->dataPointers[i];
            parent->dataPointers[i]=NULL;
        }
        for(int i=0;i<MAXCHILDREN;i++){
            pooledChildren[i]=parent->children[i];
            parent->children[i]=NULL;
        }
        parent->numKeys=0;
        //inserting median and ptrs from child
        for(int i=MAXCHILDREN;i>childIndex+1;i--){
            pooledDataPtrs[i-1]=pooledDataPtrs[i-2];
            pooledChildren[i]=pooledChildren[i-1];
        }
        pooledChildren[childIndex]=ptr1_child;
        pooledChildren[childIndex+1]=ptr2_child;
        pooledDataPtrs[childIndex]=medianDataPtrFromChild;
        //time to split the pooled array
        int medianIndex=(MAXKEYS+1)/2;
        //?filling parent 1
        //keys: [0,medianIndex-1]
        //children : [0,medianIndex]
        int k=0;
        while(k<medianIndex){
            parent->dataPointers[k]=pooledDataPtrs[k];
            parent->children[k]=pooledChildren[k];
            k++;
        }
        parent->children[k]=pooledChildren[k];
        //?filling parent 2
        //keys: [medianIndex+1,Maxkeys]
        //children: [medianIndex+1,MaxChildren]
        k=k+1;//medianIndex+1
        int j=0;
        while(k<MAXKEYS+1){
            parent2->dataPointers[j]=pooledDataPtrs[k];
            parent2->children[j]=pooledChildren[k];
            j++;
            k++;
        }
        parent2->children[j]=pooledChildren[k];
        //updating the size
        parent->numKeys=medianIndex;//!this was MAXKEYS/2
        parent2->numKeys=(MAXKEYS - parent->numKeys);
        //returning the splitted pointers
        *newMedianDataPtr=pooledDataPtrs[medianIndex];
        *ptr1_split_db=parent;
        *ptr2_split_db=parent2;
        splitted=True;
    }
    return splitted;
}

void insertMedianAndChildPtrsInParent(BTreeNode* parent,int childIndex,void* medianDataPtrFromChild,BTreeNode* ptr1_child, BTreeNode* ptr2_child){
    parent->children[childIndex]=ptr1_child;//not required but still
    //now we have to shift the ptrs and keys after child to make space for median and its child
    for(int j=MAXCHILDREN-1;j>childIndex+1;j--){
        parent->dataPointers[j-1]=parent->dataPointers[j-2];
        parent->children[j]=parent->children[j-1];
    }
    //now we can place median
    //ptr2 will go at childIndex+1, so median will go at childIndex itself
    //childIndex=maxChildren-1 or maxchildren-2 will never occur here
    parent->children[childIndex+1]=ptr2_child;
    parent->dataPointers[childIndex]=medianDataPtrFromChild;
    parent->numKeys=parent->numKeys+1;
}

void insertInLeafNode(void* arr[],void* dataPtr, int loc){
    //the children of leaf node are null, so no need to worry about that
    for(int i=MAXKEYS-1;i>loc;i--){
        arr[i]=arr[i-1];//just shifting
    }
    arr[loc]=dataPtr;
}

Bool splitLeafNode(BTreeNode* filledNode,void* dataPtr,int insertLoc,BTreeNode** ptr1_db,BTreeNode** ptr2_db, void** medianDataPtr){
    //Leaf node so children are null
    int id=getIdOfNode(dataPtr,filledNode->type);
    Bool splitted=True;
    void* dataPointers[MAXKEYS+1];
    int i=0;
    int j=0;
    // printf("\nInsert location of %d is %d\n",id,insertLoc);
    while(i<MAXKEYS+1){
        if(i==insertLoc){
            dataPointers[i]=dataPtr;
        }
        else{
            dataPointers[i]=filledNode->dataPointers[j];
            j++;
        }
        i++;
    }
    int medianIndex=(MAXKEYS+1)/2;
    BTreeNode* node2=createBTreeNode(filledNode->type);
    if(node2==NULL){
        splitted=False;
    }
    else{
        *medianDataPtr=dataPointers[medianIndex];//retrieving the median
        i=0;
        BTreeNode* node1=filledNode;
        //updating the keys in node 1
        for(j=0;j<MAXKEYS;j++){
            if(i<medianIndex){
                node1->dataPointers[j]=dataPointers[i];
                i++;
            }
            else{
                node1->dataPointers[j]=NULL;
            }
        }
        node1->numKeys=medianIndex; //Updating node1 keys(medianIndex-1)-0+1
        i++;//skipping the median index
        for(j=0;j<MAXKEYS;j++){
            if(i<MAXKEYS+1){
                node2->dataPointers[j]=dataPointers[i];
                i++;
            }
            else{
                node2->dataPointers[j]=NULL;
            }
        }
        node2->numKeys=(MAXKEYS-medianIndex);//n=maxKeys+1, numKeys=(n-1-medianIndex)
        *ptr1_db=node1;
        *ptr2_db=node2;
        splitted=True;
    }
    return splitted;
}
//? insert related functions end


//! deleting function starts

Bool deleteKey(BTree* tree, int key){
    Bool deleted=true;
    if(tree==NULL || tree->root==NULL){
        deleted=false;
    }
    else{
        deleted=removeKey(tree->root,key);
        if(tree->root->numKeys==0){ // the tree may still shrink if a key is found or not
            BTreeNode* temp=tree->root;
            Bool isRootLeaf=(temp->children[0]==NULL);//is the root node a leaf node
            if(isRootLeaf){
                tree->root=NULL;
            }
            else{
                tree->root=tree->root->children[0];//tree shrinks
            }
            free(temp);
        }
    }
    return deleted;
}

Bool removeKey(BTreeNode* node, int key) {
    Bool deleted=False;
    int idx = 0;
    while (idx < node->numKeys && getIdOfNode(node->dataPointers[idx],node->type) < key){
        idx++;
    }
    // If key is found in this node
    if (idx < node->numKeys && getIdOfNode(node->dataPointers[idx],node->type) == key) {
        // Check if node is a leaf
        Bool isLeaf = (node->children[0] == NULL);
        
        if (isLeaf) {
            // Remove key from leaf node
            deleteDataPtr(node->dataPointers[idx],node->type);
            for (int i = idx + 1; i < node->numKeys; i++){
                node->dataPointers[i-1] = node->dataPointers[i];
            }
            node->dataPointers[node->numKeys-1]=NULL;
            node->numKeys--;
            deleted= True;
        } else {
            // Node is not a leaf, handle non-leaf removal
            deleted = removeFromNonLeaf(node, idx, key);
        }
    } else {
        // Key not found in this node
        Bool isLeaf = (node->children[0] == NULL);
        
        if (isLeaf) {
            // Key not in tree
            deleted = False;
        }
        else{
            // Check if we need to adjust before recursing
            Bool isLastChild = (idx == node->numKeys);
            
            // If the child has less than minimum keys, fill it
            if (node->children[idx]->numKeys <= MINKEYS){
                fillChild(node, idx);
            }
            // If the last child has been merged, go to the previous child
            if (isLastChild && idx > node->numKeys){
                deleted = removeKey(node->children[idx-1], key);
            }
            else{
                deleted = removeKey(node->children[idx], key);
            }
        }
    }
    return deleted;
}

Bool removeFromNonLeaf(BTreeNode* node, int idx, int key) {
    Bool deleted=False;
    // printf("In remove from non leaf\n");
    // Case 1: If the child preceding the key has at least MINKEYS+1 keys
    if (node->children[idx]->numKeys >= MINKEYS+1) {
        // Find predecessor
        // printf("Getting pred\n");
        void* pred = getPredecessor(node, idx);
        copyData(node->dataPointers[idx],pred,node->type);
        // node->dataPointers[idx] = pred;//!copy the predecessors contents into this, then delete predecessor
        deleted = removeKey(node->children[idx], getIdOfNode(pred,node->type));
    }
    
    // Case 2: If the child following the key has at least MINKEYS+1 keys
    else if (node->children[idx+1]->numKeys >= MINKEYS+1) {
        // Find successor
        // printf("Getting successor");
        void* succ = getSuccessor(node, idx);
        // printf("Got successor\n");
        copyData(node->dataPointers[idx],succ,node->type);
        // printf("successor data copied\n");
        // node->dataPointers[idx] = succ;
        deleted = removeKey(node->children[idx+1], getIdOfNode(succ,node->type));
    }
    
    // Case 3: Both immediate children have only MINKEYS keys, merge them
    else {
        mergeNodes(node, idx);
        deleted = removeKey(node->children[idx], key);
    }
    return deleted;
}

void* getPredecessor(BTreeNode* node, int idx) {
    // Keep moving to the rightmost node until a leaf is reached
    BTreeNode* current = node->children[idx];
    while (current->children[0] != NULL){
        current = current->children[current->numKeys];
    }
    // Return the last key
    return current->dataPointers[current->numKeys-1];
}

// Get successor of keys[idx]
void* getSuccessor(BTreeNode* node, int idx) {
    // Keep moving to the leftmost node until a leaf is reached
    BTreeNode* current = node->children[idx+1];
    while (current->children[0] != NULL){
        current = current->children[0];
    }
    // Return the first key
    return current->dataPointers[0];
}

void fillChild(BTreeNode* node, int idx) {
    // If previous child has more than minimum keys, borrow from it
    if (idx != 0 && node->children[idx-1]->numKeys >= MINKEYS+1){
        borrowFromPrev(node, idx);
    }
    // If next child has more than minimum keys, borrow from it
    else if (idx != node->numKeys && node->children[idx+1]->numKeys >= MINKEYS+1){
        borrowFromNext(node, idx);
    }
    // Merge with a sibling
    else {
        if (idx != node->numKeys){
            mergeNodes(node, idx);
        }
        else{
            mergeNodes(node, idx-1);
        }
    }
}

void borrowFromPrev(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx-1];
    
    // Move all keys in child one position ahead
    for (int i = child->numKeys-1; i >= 0; i--){
        child->dataPointers[i+1] = child->dataPointers[i];
    }
    // If child is not a leaf, move all its child pointers one step ahead
    if (child->children[0] != NULL) { 
        for (int i = child->numKeys; i >= 0; i--){
            child->children[i+1] = child->children[i];
        }
    }
    
    // Set child's first key equal to the parent's key at idx-1
    child->dataPointers[0] = node->dataPointers[idx-1];
    
    // Moving sibling's last child as child's first child
    if (child->children[0] != NULL){
        child->children[0] = sibling->children[sibling->numKeys];
    }
    
    // Moving the key from sibling to parent
    node->dataPointers[idx-1] = sibling->dataPointers[sibling->numKeys-1];
    //making the sibling last child and last data pointer null
    sibling->dataPointers[sibling->numKeys-1]=NULL;
    sibling->children[sibling->numKeys]=NULL;
    //changing the numkeys in child and sibling
    child->numKeys += 1;
    sibling->numKeys -= 1;
}

// Borrow a key from next child
void borrowFromNext(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx+1];
    
    // Copy parent's key to child
    child->dataPointers[child->numKeys] = node->dataPointers[idx];
    
    // If not a leaf, move sibling's first child to child's last child
    if (child->children[0] != NULL){
        child->children[child->numKeys+1] = sibling->children[0];
    }
    // Copy sibling's first key to parent
    node->dataPointers[idx] = sibling->dataPointers[0];
    
    // Move all dataPointers in sibling one step behind
    for (int i = 1; i < sibling->numKeys; i++){
        sibling->dataPointers[i-1] = sibling->dataPointers[i];
    }
    // Move all child pointers one step behind if not a leaf
    if (sibling->children[0] != NULL) {
        for (int i = 1; i <= sibling->numKeys; i++){
            sibling->children[i-1] = sibling->children[i];
        }
    }
    //making the sibling last child and last data pointer null as they are shifted
    sibling->dataPointers[sibling->numKeys-1]=NULL;
    sibling->children[sibling->numKeys]=NULL;
    
    child->numKeys += 1;
    sibling->numKeys -= 1;
}

// Merge node's children at idx and idx+1
void mergeNodes(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx+1];
    
    // Copy key from parent to child at (MINKEYS)th position
    child->dataPointers[MINKEYS] = node->dataPointers[idx];
    
    // Copy all keys from sibling to child
    for (int i = 0; i < sibling->numKeys; i++){
        child->dataPointers[i+MINKEYS+1] = sibling->dataPointers[i];
    }
    // Copy child pointers from sibling to child
    if (child->children[0] != NULL) {
        for (int i = 0; i <= sibling->numKeys; i++){
            child->children[i+MINKEYS+1] = sibling->children[i];
        }
    }
    
    // Move all keys after idx in the parent node one step before
    for (int i = idx+1; i < node->numKeys; i++){
        node->dataPointers[i-1] = node->dataPointers[i];
    }
    node->dataPointers[node->numKeys-1]=NULL;//added
    //making the last data pointer null as the index/key has been removed
    // Move the child pointers after (idx+1) in the parent node one step before
    for (int i = idx+2; i <= node->numKeys; i++){
        node->children[i-1] = node->children[i];
    }
    node->children[node->numKeys]=NULL;//as one child is gone, making it null
    // Update key count of child and parent
    child->numKeys += sibling->numKeys + 1;
    node->numKeys-=1;
    // Free memory occupied by sibling
    free(sibling);
}

void deleteDataPtr(void* dataPtr,NodeType type){
    switch (type){
        case FAMILY:{
            Family* family_ptr=(Family*)dataPtr;
            for(int i=0;i<family_ptr->numUsers;i++){
                deleteExpenseDb(&(family_ptr->users[i]->expenses));
                free(family_ptr->users[i]);
                // printf("Deleted user\n");
            }
            free(family_ptr);
            // printf("Deleted Family Node\n");
        }
        break;
        case EXPENSE:{
            free((Expense*)dataPtr);
            // printf("Deleted Expense Node\n");
        }
        break;
        case USER:{
            free((User*)dataPtr);
            // printf("Deleted User Node\n");
        }
        break;
    }
}

void copyData(void* node1, void* node2, NodeType type) {
    if (!node1 || !node2) return; // Ensure pointers are valid

    switch (type) {
        case FAMILY: {
            Family* dest = (Family*)node1;
            Family* src = (Family*)node2;
            
            dest->familyId = src->familyId;
            strcpy(dest->familyName, src->familyName);
            dest->numUsers = src->numUsers;
            dest->totalIncome = src->totalIncome;
            dest->totalExpense = src->totalExpense;

            // Copy user pointers (shallow copy)
            for (int i = 0; i < src->numUsers; i++) {
                dest->users[i] = src->users[i];
            }
        }
        break;

        case EXPENSE: {
            Expense* dest = (Expense*)node1;
            Expense* src = (Expense*)node2;

            dest->expenseId = src->expenseId;
            dest->userId = src->userId;
            dest->category = src->category;
            dest->amount = src->amount;
            dest->date = src->date;
        }
        break;

        case USER: {
            User* dest = (User*)node1;
            User* src = (User*)node2;

            dest->userId = src->userId;
            strcpy(dest->userName, src->userName);
            dest->income = src->income;

            // Copy expense head pointer (shallow copy)
            dest->expenses = src->expenses;
        }
        break;
    }
}

//! deleting function ends

//! search functions

void* searchBTree(BTreeNode* root, int key) {
    void* ret_val=NULL;
    if (root != NULL){
        int i = 0;
        while (i < root->numKeys && key > getIdOfNode(root->dataPointers[i], root->type)) {
            i++;
        }
        if (i < root->numKeys && key == getIdOfNode(root->dataPointers[i], root->type)) {
            ret_val = root->dataPointers[i];
        }
        else if (root->children[i] != NULL) {
            ret_val = searchBTree(root->children[i], key);
        }   
    }
    return ret_val; 
}



//! search functions end


void deleteDataNodes(BTreeNode* root){
    for(int i=0;i<root->numKeys;i++){
        switch(root->type){
            case FAMILY:{
                Family* node=(Family*)root->dataPointers[i];
                for(int i=0;i<node->numUsers;i++){
                    deleteExpenseDb(&(node->users[i]->expenses));
                    free(node->users[i]);
                    // printf("Deleted user\n");
                }
                free(node);
                // printf("Deleted Family data node\n");
            }
            break;
            case EXPENSE:{
                Expense* node=(Expense*)root->dataPointers[i];
                free(node);
                // printf("Deleted Expense data node\n");
            }
            break;
            case USER:{
                User* node=(User*)root->dataPointers[i];
                deleteExpenseDb(&(node->expenses));
                free(node);
                // printf("Deleted user data node\n");
            }
            break;
            default:{
                // printf("Failed to delete Data node, id: %d ,type: %d\n",getIdOfNode(root->dataPointers[i],root->type),root->type);
            }
        }
    }
}

void deleteNodeRec(BTreeNode* root){
    if(root!=NULL){
        deleteDataNodes(root);
        for(int i=0;i<root->numKeys+1;i++){
            deleteNodeRec(root->children[i]);
            // printf("Deleting node\n");
        }
        free(root);
    }
}

void deleteBTree(BTree* tree){
    if(tree!=NULL){
        deleteNodeRec(tree->root);
        tree->root=NULL;
    }
}

void visitDataPointer(void * dataNode,NodeType type){
    switch (type) {
        case FAMILY:
            printFamily(dataNode);
            break;
        case USER:
            printUser(dataNode);
            break;
        case EXPENSE:
            printExpense(dataNode);
            break;
        default:{
            printf("\nInvalid type of Data\n");
        }
    }
}

void printBTreeInOrder(BTreeNode* root) {
    if (root == NULL) return;

    for (int i = 0; i < root->numKeys; i++) {
        if (root->children[i] != NULL) {
            printBTreeInOrder(root->children[i]);
        }
        visitDataPointer(root->dataPointers[i], root->type);
    }
    if (root->children[root->numKeys] != NULL) {
        printBTreeInOrder(root->children[root->numKeys]);
    }
}

//just prints id
void printBFS(BTree* tree) {
    if (!tree->root) return;
    
    Queue q;
    initQueue(&q);
    push(&q, tree->root);
    
    while (!isEmpty(&q)) {
        int levelSize = getSize(&q);
        
        for (int i = 0; i < levelSize; i++) {
            BTreeNode* node = front(&q);
            pop(&q);
            
            printf("[");
            for (int j = 0; j < node->numKeys; j++) {
                printf("%d%s", getIdOfNode(node->dataPointers[j],node->type), (j < node->numKeys - 1) ? ", " : "");
            }
            printf("] ");
            
            for (int j = 0; j <= node->numKeys; j++) {
                if (node->children[j]) {
                    push(&q, node->children[j]);
                }
            }
        }
        printf("\n");
    }
    clear(&q);
}