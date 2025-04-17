#ifndef USER_H
#define USER_H
#include "b_tree.h"
#include "expense.h"

typedef struct User_Tag{
    int userId;
    char userName[50];
    float income;
    Expense_DB expenses;
}User;

void printUser(User* user);
Expense* searchExpenseInUser(User* user, int expenseId);
void printExpensesOfUser(User* user);
Bool deleteExpenseFromUserExpenses(User* user,int expenseId);

#endif