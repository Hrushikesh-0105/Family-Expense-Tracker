#include "user.h"
#include <string.h>
#include <stdlib.h>
#include<stdio.h>


void printUser(User* user){
    printf("User Id: %d\n",user->userId);
    printf("Name: %s\n",user->userName);
    printf("Income: %.2f\n",user->income);
}

Expense* searchExpenseInUser(User* user, int expenseId){
    return searchBTree(user->expenses.tree.root,expenseId);
}

void printExpensesOfUser(User* user){
    printBTreeInOrder(user->expenses.tree.root);//printing expenses
}

Bool deleteExpenseFromUserExpenses(User* user,int expenseId){
    Bool deleted=False;
    if(user!=NULL){
        deleted=deleteKey(&(user->expenses.tree),expenseId);
    }
    return deleted;
}
