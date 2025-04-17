#include "file_ops.h"
#include <stdio.h>

void inOrderBTree(BTreeNode* root,FILE* file);

void inputDataFromFile(Family_DB* family_db_ptr){
    printf("Loading Data\n");
    FILE * file=fopen(FILENAME,"r");
    if(file==NULL){
        printf("Failed to open File\n");
    }
    else{
        char line[100];
        Family newFamily;
        User newUser;
        Expense newExpense;
        Family* currentFamilyPtr=NULL;
        User* currentUserPtr=NULL;
        char identifier;
        while(fgets(line, sizeof(line), file)){
            if(line[0]=='f'){
                currentFamilyPtr=NULL;
                sscanf(line,"%c %d %s",&identifier,&newFamily.familyId,&newFamily.familyName);
                newFamily.numUsers=0;
                newFamily.totalExpense=0;
                newFamily.totalIncome=0;
                currentFamilyPtr=insertFamilyFromFile(family_db_ptr,&newFamily);
            }
            else if(line[0]=='u'){
                currentUserPtr=NULL;
                sscanf(line,"%c %d %s %f",&identifier,&newUser.userId,&newUser.userName,&newUser.income);
                currentUserPtr=insertUserInFamilyFromFile(currentFamilyPtr,&newUser);
            }
            else if(line[0]=='e'){
                sscanf(line,"%c %d %d %d %f %d",&identifier, &newExpense.userId,&newExpense.expenseId,&newExpense.category,&newExpense.amount,&newExpense.date);
                insertExpenseInUserFromFile(currentFamilyPtr,currentUserPtr,&newExpense);
            }
        }
    }
    fclose(file);
}

void outputDataToFile(Family_DB* family_db_ptr){
    printf("Storing Data\n");
    FILE * file=fopen(FILENAME,"w");
    if(file==NULL){
        printf("Failed to open File\n");
    }
    else{
        inOrderBTree(family_db_ptr->tree.root,file);
    }
    printf("Data stored successfully\n");
    fclose(file);
}

void saveDataPtr(void * dataNode,NodeType type,FILE* file){
    switch (type) {
        case FAMILY:{
            Family* familyNode=(Family*)dataNode;
            fprintf(file,"\nf %d %s\n",familyNode->familyId,familyNode->familyName);
            for(int i=0;i<familyNode->numUsers;i++){
                User* userNode=familyNode->users[i];
                fprintf(file,"u %d %s %.2f\n",userNode->userId,userNode->userName,userNode->income);
                inOrderBTree(userNode->expenses.tree.root,file);
            }
        }
            break;
        case EXPENSE:{
            Expense* expenseNode=(Expense*) dataNode;
            fprintf(file,"e %d %d %d %.2f %d\n",expenseNode->userId,expenseNode->expenseId,expenseNode->category,expenseNode->amount,expenseNode->date);
        }
            break;
        default:{
            printf("\nInvalid type of Data\n");
        }
    }
}

void inOrderBTree(BTreeNode* root,FILE* file) {
    if (root == NULL) return;

    for (int i = 0; i < root->numKeys; i++) {
        if (root->children[i] != NULL) {
            inOrderBTree(root->children[i],file);
        }
        saveDataPtr(root->dataPointers[i], root->type,file);
    }
    if (root->children[root->numKeys] != NULL) {
        inOrderBTree(root->children[root->numKeys],file);
    }
}