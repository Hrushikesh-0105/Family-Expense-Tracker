#include "family.h"
#include "expense.h"
#include <string.h>
#include <stdlib.h>
#include<stdio.h>

void heapSortShortExpenses(ShortExpense arr[], int n);
void heapify(ShortExpense arr[], int n, int i);

void initializeFamilyDb(Family_DB* db){
    db->AutoIncrementId=0;
    initializeBTree(&(db->tree),FAMILY);
}

void deleteFamilyDb(Family_DB* db){
    db->AutoIncrementId=0;
    deleteBTree(&(db->tree));
}


Family* searchFamily(Family_DB* family_db_ptr,int familyId){
    Family* ret_family=NULL;
    if(family_db_ptr!=NULL && familyId>0){
        ret_family=searchBTree(family_db_ptr->tree.root,familyId);
    }
    return ret_family;
}

User* searchUserInFamily(Family* family_ptr, int userId){
    User* user=NULL;
    if(family_ptr!=NULL){
        for(int i=0;i<family_ptr->numUsers && user==NULL;i++){
            if(family_ptr->users[i]->userId==userId){
                user=family_ptr->users[i];
            }
        }
    }
    return user;
}



Family* insertFamily(Family_DB* db,Family* newFamilyTemp){
    //dynamically allocated Family pointer will be returned back
    Family* newFamily=NULL;
    newFamily=(Family*)malloc(sizeof(Family));
    if(newFamily){
        newFamily->familyId=db->AutoIncrementId+1;
        strcpy(newFamily->familyName,newFamilyTemp->familyName);
        newFamily->totalIncome=0;
        newFamily->totalExpense=0;
        newFamily->numUsers=0;
        //insert in the b tree
        if(db->tree.root==NULL){
            initializeBTree(&(db->tree),FAMILY);
        }
        Bool inserted=insertKey(&(db->tree),newFamily);
        if(inserted){
            printf("\nFamily inserted successfully\n");
            db->AutoIncrementId=db->AutoIncrementId+1;//incrementing the id for next insert
        }else{
            printf("Failed to insert Family\n");
            free(newFamily);
            newFamily=NULL;
        }
    }
    return newFamily;
}

User* insertUserInFamily(Family* familyPtr, User* user_ptr_static) {
    User* newUser;
    if (familyPtr == NULL || user_ptr_static == NULL){
        newUser=NULL;
    }
    else if (familyPtr->numUsers >= MAX_USERS_IN_FAMILY){
        newUser=NULL;
    }
    else{
        newUser = (User*) malloc(sizeof(User));
        if (newUser != NULL){
            *newUser = *user_ptr_static;
            initializeExpenseDb(&(newUser->expenses));  
            if (familyPtr->numUsers == 0) {
                newUser->userId = 1;
            } else {
                newUser->userId = familyPtr->users[familyPtr->numUsers - 1]->userId + 1;
            }
            familyPtr->users[familyPtr->numUsers] = newUser;
            familyPtr->totalIncome+=newUser->income;//incrementing income of family
            familyPtr->numUsers++;
        }
    }
    return newUser;
}

Expense* insertExpenseInUser(Family*familyPtr,User* userPtr,Expense* expense_static_ptr){
    Expense* newExpense=NULL;
    if(familyPtr!=NULL && userPtr!=NULL && expense_static_ptr!=NULL){
       newExpense=insertExpense(&(userPtr->expenses),expense_static_ptr);
    }
    if(newExpense!=NULL){
        familyPtr->totalExpense+=newExpense->amount;
    }
    return newExpense;
}

Bool deleteFamily(Family_DB* family_db_ptr,int familyId){
    Bool deleted=False;
    if(family_db_ptr!=NULL && familyId>0){
        deleted=deleteKey(&(family_db_ptr->tree),familyId);
    }
    return deleted;
}

Bool removeUserFromFamily(Family* family,int userId){
    Bool deleted=True;
    if(userId<=0){
        deleted=False;
    }
    else{
        int userIndex=-1;
        for(int i=0;i<family->numUsers &&  userIndex==-1;i++){
            if(family->users[i]->userId==userId){
                userIndex=i;
            }
        }
        if(userIndex==-1){
            deleted=False;
        }
        else{
            User* user=family->users[userIndex];
            float userIncome=user->income;
            float totalExpenseOfUser=0.0;
            totalExpenseOfUser=traverseAndcalculateTotalExpenseOfUser(&(user->expenses));
            family->totalExpense=family->totalExpense-totalExpenseOfUser;
            family->totalIncome=family->totalIncome-userIncome;
            deleteExpenseDb(&(user->expenses));
            free(user);
            family->users[userIndex]=NULL;
            for(int i=userIndex;i<family->numUsers-1;i++){
                family->users[i]=family->users[i+1];
            }
            family->users[family->numUsers-1]=NULL;
            family->numUsers=family->numUsers-1;
            deleted=True;
        }
    }
    return deleted;
}

//Home page calculation functions
void calculateCategoricalExpense(Family* current_family_ptr, ExpenseCategory category,ShortExpense expensesOfUsers[]){
    for(int i=0;i<current_family_ptr->numUsers;i++){
        expensesOfUsers[i].category=category;
        expensesOfUsers[i].userId=current_family_ptr->users[i]->userId;
        expensesOfUsers[i].totalExpense=traverseAndcalculateTotalExpenseOfUserInCategory(&(current_family_ptr->users[i]->expenses),category);
    }
    heapSortShortExpenses(expensesOfUsers,current_family_ptr->numUsers);
}

void calculateTotalExpenseOnDays(Family* current_family_ptr,ShortExpense expensesOnDays[]){
    for(int i=0;i<MAX_DAYS;i++){
        expensesOnDays[i].expenseDay=i+1;
        expensesOnDays[i].totalExpense=0.00;
    }
    //for each user go through expense db and increment the amount on the particular day
    for(int i=0;i<current_family_ptr->numUsers;i++){
        traverseAndCalculateTotalExpenseOnDays(&(current_family_ptr->users[i]->expenses),expensesOnDays);
    }
    //now sort the expenses on days array in descending
    heapSortShortExpenses(expensesOnDays,MAX_DAYS);
}

void calculateAllCategoricalExpensesOfUser(User* chosenUser,ShortExpense categoricalExpensesOfUser[]){
    //for each category we have to calculate total expense
    for(int i=0;i<MAX_CATEGORIES;i++){
        categoricalExpensesOfUser[i].category=i+1;
        categoricalExpensesOfUser[i].totalExpense=0.00;
    }
    traverseAndCalculateTotalCategoricalExpenses(&(chosenUser->expenses),categoricalExpensesOfUser);
    heapSortShortExpenses(categoricalExpensesOfUser,MAX_CATEGORIES);
}

void getAllExpensesInGivenPeriod(Family* family,ExpenseLinkedList expensesListInDate[],int startDate,int endDate){
    for(int i=0;i<family->numUsers;i++){
        traverseAndGetExpensesWithinPeriod(&(family->users[i]->expenses),expensesListInDate,startDate,endDate);
    }
}

void getAllExpensesInIdRange(User* user,ExpenseLinkedList* expenseListPtr,int startId, int endId){
    traverseAndGetExpensesWithinIdRange(&(user->expenses),expenseListPtr,startId,endId);
}

//? sorting for the short expenses

void heapify(ShortExpense arr[], int n, int i) {
    int largest = i;          
    int left = 2 * i + 1;     
    int right = 2 * i + 2;    

    if (left < n && arr[left].totalExpense > arr[largest].totalExpense){
        largest = left;
    }
    if (right < n && arr[right].totalExpense > arr[largest].totalExpense){
        largest = right;
    }
    if (largest != i) {
        ShortExpense temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        heapify(arr, n, largest);
    }
}

// main heap sort function
void heapSortShortExpenses(ShortExpense arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--){
        heapify(arr, n, i);
    }
    for (int i = n - 1; i >= 0; i--) {
        ShortExpense temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        heapify(arr, i, 0);
    }
    //reversing the array for descending order
    for (int i = 0; i < n / 2; i++) {
        ShortExpense temp = arr[i];
        arr[i] = arr[n - i - 1];
        arr[n - i - 1] = temp;
    }
}



//?sorting function ends




//printing stuff

void printFamilyDb(Family_DB* db){
    printBTreeInOrder(db->tree.root);
}

void printFamily(Family* family){
    printf("Family Id: %d\n",family->familyId);
    printf("Family Name: %s\n",family->familyName);
    printf("Total Income: %.2f\n",family->totalIncome);
    printf("Total Expense: %.2f\n",family->totalExpense);
    printf("Users: ");
    for(int i=0;i<family->numUsers;i++){
        printf("%d ",family->users[i]->userId);
    }
}

void printFullFamilyDetails(Family* familyPtr){
    if(familyPtr!=NULL){
        printf("\n************************\n");
        printFamily(familyPtr);
        printf("\n------------------------\n");
        for(int i=0;i<familyPtr->numUsers;i++){
            printUser(familyPtr->users[i]);//printing users
            printBTreeInOrder(familyPtr->users[i]->expenses.tree.root);//printing expenses
            printf("------------------------\n");
        }
        printf("***********************\n");
    }
}

void printUsersOfFamily(Family* family_ptr){
    if(family_ptr!=NULL){
        printf("\n************************\n");
        printFamily(family_ptr);
        printf("\n------------------------\n");
        for(int i=0;i<family_ptr->numUsers;i++){
            printUser(family_ptr->users[i]);//printing users
            printf("------------------------\n");
        }
        printf("************************\n");
    }
}

//file ops
int max(int a, int b){
    return a>b?a:b;
}

Family* insertFamilyFromFile(Family_DB* db,Family* newFamilyTemp){
    Family* newFamily=NULL;
    newFamily=(Family*)malloc(sizeof(Family));
    if(newFamily){
        newFamily->familyId=newFamilyTemp->familyId;//taking family id from the file
        strcpy(newFamily->familyName,newFamilyTemp->familyName);
        newFamily->totalIncome=0;
        newFamily->totalExpense=0;
        newFamily->numUsers=0;
        //insert in the b tree
        if(db->tree.root==NULL){
            initializeBTree(&(db->tree),FAMILY);
        }
        Bool inserted=insertKey(&(db->tree),newFamily);
        if(inserted){
            // printf("\nFamily inserted successfully (File ops)\n");
            db->AutoIncrementId=max(db->AutoIncrementId,newFamily->familyId);//incrementing the id for next insert
        }else{
            printf("Failed to insert Family (File ops)\n");
            free(newFamily);
            newFamily=NULL;
        }
    }
    return newFamily;
}

User* insertUserInFamilyFromFile(Family* familyPtr,User* newUserTemp){
    User* newUser;
    if (familyPtr == NULL || newUserTemp == NULL){
        newUser=NULL;
    }
    else if (familyPtr->numUsers >= MAX_USERS_IN_FAMILY){
        newUser=NULL;
    }
    else{
        newUser = (User*) malloc(sizeof(User));
        if (newUser != NULL){
            *newUser = *newUserTemp;//copies the id from the file too
            initializeExpenseDb(&(newUser->expenses));  
            familyPtr->users[familyPtr->numUsers] = newUser;
            familyPtr->totalIncome+=newUser->income;//incrementing income of family
            familyPtr->numUsers++;
            // printf("User inserted Successfully(File ops)");
        }
        else{
            printf("Failed to insert user(File ops)");
        }
    }
    return newUser;
}

Bool insertExpenseInUserFromFile(Family*familyPtr,User* userPtr,Expense* newExpenseTemp){
    Expense* newExpense=NULL;
    Bool expenseInserted=False;
    if(familyPtr!=NULL && userPtr!=NULL && newExpenseTemp!=NULL){
        newExpense = (Expense*)malloc(sizeof(Expense));
        if(newExpense!=NULL){
            *newExpense=*newExpenseTemp;//copy every field even expense id and user id
            if (userPtr->expenses.tree.root == NULL) {
                initializeBTree(&(userPtr->expenses.tree), EXPENSE);
            }
            expenseInserted = insertKey(&(userPtr->expenses.tree), newExpense);
            if (expenseInserted) {
                // printf("\nExpense inserted successfully(File ops)\n");
                userPtr->expenses.AutoIncrementId=max(userPtr->expenses.AutoIncrementId,newExpense->expenseId);
            } else {
                free(newExpense); 
                newExpense = NULL;
            }
        }
        if(newExpense!=NULL && expenseInserted){
            familyPtr->totalExpense+=newExpense->amount;
        }
    }
    return expenseInserted;
}