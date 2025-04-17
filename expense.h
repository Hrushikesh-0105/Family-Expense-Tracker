#ifndef EXPENSE_H
#define EXPENSE_H

#include "b_tree.h"
// #include "linked_list.h"
// #include "family.h"
typedef struct ExpenseLinkedList ExpenseLinkedList;
#include "linked_list.h"

typedef enum {RENT=1,UTILITY,GROCERY,STATIONARY,LEISURE} ExpenseCategory;
#define MAX_CATEGORIES 5

typedef struct Expense_Tag{
    int userId;
    int expenseId;
    ExpenseCategory category;
    float amount;
    int date;
}Expense;

typedef struct Expense_DB_Tag{
    int AutoIncrementId;
    BTree tree;
}Expense_DB;

typedef struct ShortExpense{
    int userId;
    float totalExpense;
    ExpenseCategory category;
    int expenseDay;
}ShortExpense;

void initializeExpenseDb(Expense_DB* db);

const char* getCategoryName(ExpenseCategory category);

Expense* insertExpense(Expense_DB* db, Expense* newExpenseTemp);
void deleteExpenseDb(Expense_DB* db);

float traverseAndcalculateTotalExpenseOfUser(Expense_DB* db);
float traverseAndcalculateTotalExpenseOfUserInCategory(Expense_DB* db, ExpenseCategory category);
void traverseAndCalculateTotalExpenseOnDays(Expense_DB* db, ShortExpense expensesOnDays[]);
void traverseAndCalculateTotalCategoricalExpenses(Expense_DB* db, ShortExpense categoricalExpensesOfUser[]);
void traverseAndGetExpensesWithinPeriod(Expense_DB* db,ExpenseLinkedList expensesListInDate[],int startDate,int endDate);
void traverseAndGetExpensesWithinIdRange(Expense_DB* db,ExpenseLinkedList* expensesInIdRangeList,int startId,int endId);

void printExpense(Expense* exp);



#endif