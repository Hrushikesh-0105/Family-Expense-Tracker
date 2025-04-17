#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "expense.h"

void initializeExpenseLinkedList(ExpenseLinkedList* list) {
    list->head = NULL;
    list->size = 0;
}

void insertAtHeadExpenseList(ExpenseLinkedList* list, Expense* expense) {
    ExpenseListNode* newNode = (ExpenseListNode*) malloc(sizeof(ExpenseListNode));
    newNode->expense = expense;
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

void insertSortedDescendingExpenseList(ExpenseLinkedList* list, Expense* expense) {
    ExpenseListNode* newNode = (ExpenseListNode*) malloc(sizeof(ExpenseListNode));
    newNode->expense = expense;
    newNode->next = NULL;
    if (list->head == NULL || list->head->expense->amount < expense->amount) {
        newNode->next = list->head;
        list->head = newNode;
    } else {
        ExpenseListNode* current = list->head;
        while (current->next != NULL && current->next->expense->amount >= expense->amount) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    list->size++;
}

void deleteExpenseLinkedList(ExpenseLinkedList* list) {
    ExpenseListNode* current = list->head;
    while (current != NULL) {
        printf("freeing up node\n");
        ExpenseListNode* temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
    list->size = 0;
}

void printExpenseLinkedList(const ExpenseLinkedList* list) {
    ExpenseListNode* current = list->head;
    while (current != NULL) {
        printExpense(current->expense);
        current = current->next;
    }
}

