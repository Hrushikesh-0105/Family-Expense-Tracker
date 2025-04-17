#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Expense_Tag Expense;

typedef struct ExpenseListNode {
    Expense* expense;
    struct ExpenseListNode* next;
} ExpenseListNode;

typedef struct ExpenseLinkedList {
    ExpenseListNode* head;
    int size;
} ExpenseLinkedList;

void initializeExpenseLinkedList(ExpenseLinkedList* list);

void insertAtHeadExpenseList(ExpenseLinkedList* list, Expense* expense);
void insertSortedDescendingExpenseList(ExpenseLinkedList* list, Expense* expense);

void deleteExpenseLinkedList(ExpenseLinkedList* list);
void printExpenseLinkedList(const ExpenseLinkedList* list);

#endif