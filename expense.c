#include "expense.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void initializeExpenseDb(Expense_DB* db) {
    db->AutoIncrementId = 0;
    initializeBTree(&(db->tree), EXPENSE); 
}

void deleteExpenseDb(Expense_DB* db) {
    db->AutoIncrementId = 0;
    deleteBTree(&(db->tree));
}

const char* getCategoryName(ExpenseCategory category) {
    switch (category) {
        case RENT: return "Rent";
        case UTILITY: return "Utility";
        case GROCERY: return "Grocery";
        case STATIONARY: return "Stationary";
        case LEISURE: return "Leisure";
        default: return "Unknown";
    }
}

void printExpense(Expense* exp){
    printf("\n$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("Expense ID: %d\n", exp->expenseId);
    printf("User ID: %d\n", exp->userId);
    printf("Category: %s\n", getCategoryName(exp->category));
    printf("Amount: $%.2f\n", exp->amount);
    printf("Date: %d\n", exp->date); 
    printf("$$$$$$$$$$$$$$$$$$$$$$$\n");
}

Expense* insertExpense(Expense_DB* db, Expense* newExpenseTemp) {
    Expense* newExpense = (Expense*)malloc(sizeof(Expense));
    if (newExpense) {
        newExpense->expenseId = db->AutoIncrementId + 1; 
        newExpense->userId = newExpenseTemp->userId;
        newExpense->category = newExpenseTemp->category;
        newExpense->amount = newExpenseTemp->amount;
        newExpense->date = newExpenseTemp->date;

        if (db->tree.root == NULL) {
            initializeBTree(&(db->tree), EXPENSE);
        }

        Bool inserted = insertKey(&(db->tree), newExpense);
        if (inserted) {
            printf("\nExpense inserted successfully\n");
            db->AutoIncrementId++; 
        } else {
            free(newExpense); 
            newExpense = NULL;
        }
    }
    return newExpense;
}


//Expense calculation functions

void addToTotal(Expense* expense, void* context);
void categoryVisitor(Expense* expense, void* context);
void daysVisitor(Expense* expense, void* context);
void allCategoriesVisitor(Expense* expense, void* context);
void expenseDatePeriodVisitor(Expense* expense, void* context);
void expenseIdRangeVisitor(Expense* expense, void* context);

typedef struct {
    ExpenseCategory category;
    float total;
} CategoryContext;

typedef struct {
    ShortExpense* expenseOnDays;
}DaysContext;

typedef struct {
    ShortExpense* categoricalExpensesOfUser;
}AllCategoriesContext;

typedef struct {
    ExpenseLinkedList* expensesListInDate;//array of linked lists of expense pointers
    int startDate;
    int endDate;
}ExpenseDatePeriodContext;

typedef struct {
    int startId;
    int endId;
    ExpenseLinkedList* expensesInIdRangeList;
}ExpenseIdPeriodContext;

typedef void (*ExpenseVisitor)(Expense* expense, void* context);


//actual traversal
void traverseExpenseInOrder(BTreeNode* root, ExpenseVisitor visitor, void* context) {
    if (root != NULL){
        for (int i = 0; i < root->numKeys; i++) {
            if (root->children[i] != NULL) {
                traverseExpenseInOrder(root->children[i], visitor, context);
            }
            visitor((Expense*)root->dataPointers[i], context);
        }
    
        if (root->children[root->numKeys] != NULL) {
            traverseExpenseInOrder(root->children[root->numKeys], visitor, context);
        }
    }    
}

//traversal result functions- what to do when we visit a node
void addToTotal(Expense* expense, void* context) {
    float* total = (float*)context;
    *total += expense->amount;
}

void categoryVisitor(Expense* expense, void* context) {
    CategoryContext* catContext = (CategoryContext*)context;
    if (expense->category == catContext->category) {
        catContext->total += expense->amount;
    }
}

void daysVisitor(Expense* expense, void* context){
    DaysContext* daysContext= (DaysContext*) context;
    daysContext->expenseOnDays[expense->date-1].totalExpense+=expense->amount;//incrementing the day's total amount
}

void allCategoriesVisitor(Expense* expense, void* context){
    AllCategoriesContext* allCatContext= (AllCategoriesContext*) context;
    allCatContext->categoricalExpensesOfUser[expense->category-1].totalExpense+=expense->amount;//incrementing categorical expense
}

void expenseDatePeriodVisitor(Expense* expense, void* context){
    ExpenseDatePeriodContext* expensePeriodContext= (ExpenseDatePeriodContext*)context;
    int startDate=expensePeriodContext->startDate;
    int endDate=expensePeriodContext->endDate;
    if(expense->date>=startDate && expense->date<=endDate){
        insertSortedDescendingExpenseList(&(expensePeriodContext->expensesListInDate[expense->date-startDate]),expense);
    }
}

void expenseIdRangeVisitor(Expense* expense, void* context){
    ExpenseIdPeriodContext* expenseIdRangeContext= (ExpenseIdPeriodContext*)context;
    int startId=expenseIdRangeContext->startId;
    int endId=expenseIdRangeContext->endId;
    if(expense->expenseId>=startId && expense->expenseId<=endId){
        insertAtHeadExpenseList(expenseIdRangeContext->expensesInIdRangeList,expense);
    }
}


//custom traversals
float traverseAndcalculateTotalExpenseOfUser(Expense_DB* db) {
    float totalExpense = 0.0;
    traverseExpenseInOrder(db->tree.root, addToTotal, &totalExpense);
    return totalExpense;
}

float traverseAndcalculateTotalExpenseOfUserInCategory(Expense_DB* db, ExpenseCategory category){
    CategoryContext context;
    context.total=0.0;
    context.category=category;
    traverseExpenseInOrder(db->tree.root, categoryVisitor, &context);
    return context.total;
}

void traverseAndCalculateTotalExpenseOnDays(Expense_DB* db, ShortExpense expensesOnDays[]){
    DaysContext daysContext;
    daysContext.expenseOnDays=expensesOnDays;
    traverseExpenseInOrder(db->tree.root,daysVisitor,&daysContext);
}

void traverseAndCalculateTotalCategoricalExpenses(Expense_DB* db, ShortExpense categoricalExpensesOfUser[]){
    AllCategoriesContext context;
    context.categoricalExpensesOfUser=categoricalExpensesOfUser;
    traverseExpenseInOrder(db->tree.root,allCategoriesVisitor,&context);
}

void traverseAndGetExpensesWithinPeriod(Expense_DB* db,ExpenseLinkedList expensesListInDate[],int startDate,int endDate){
    ExpenseDatePeriodContext context;
    context.startDate=startDate;
    context.endDate=endDate;
    context.expensesListInDate=expensesListInDate;
    traverseExpenseInOrder(db->tree.root,expenseDatePeriodVisitor,&context);
}

void traverseAndGetExpensesWithinIdRange(Expense_DB* db,ExpenseLinkedList* expensesInIdRangeList,int startId,int endId){
    ExpenseIdPeriodContext context;
    context.startId=startId;
    context.endId=endId;
    context.expensesInIdRangeList=expensesInIdRangeList;
    traverseExpenseInOrder(db->tree.root,expenseIdRangeVisitor,&context);
}