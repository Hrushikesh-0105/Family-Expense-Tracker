#ifndef FAMILY_H
#define FAMILY_H

#include "b_tree.h"
#include "user.h"
#include "linked_list.h"

#define MAX_USERS_IN_FAMILY 4
#define MAX_DAYS 10

typedef struct Family_Tag {
    int familyId;
    char familyName[50];
    User* users[MAX_USERS_IN_FAMILY];
    int numUsers;
    float totalIncome;
    float totalExpense;
} Family;

typedef struct Family_DB_Tag{
    int AutoIncrementId;
    BTree tree;
}Family_DB;

//short expense foe expense calculations



//main functions
void initializeFamilyDb(Family_DB* db);

Family* insertFamily(Family_DB* db,Family* family_ptr_static);
User* insertUserInFamily(Family* familyPtr,User* user_ptr_static);
Expense* insertExpenseInUser(Family*familyPtr,User* userPtr,Expense* expense_static_ptr);


Family* searchFamily(Family_DB* familly_db_ptr, int familyId);
User* searchUserInFamily(Family* family_ptr, int userId);


Bool deleteFamily(Family_DB* family_db_ptr,int familyId);
Bool removeUserFromFamily(Family* family,int userId);

//Home page calculation functions
void calculateCategoricalExpense(Family* current_family_ptr, ExpenseCategory category,ShortExpense expensesOfUsers[]);
void calculateTotalExpenseOnDays(Family* current_family_ptr,ShortExpense expensesOnDays[]);
void calculateAllCategoricalExpensesOfUser(User* chosenUser,ShortExpense categoricalExpensesOfUser[]);
void getAllExpensesInGivenPeriod(Family* family,ExpenseLinkedList expensesListInDate[],int startDate,int endDate);
void getAllExpensesInIdRange(User* user,ExpenseLinkedList* expenseListPtr,int startId, int endId);

//helper
void deleteFamilyDb(Family_DB* db);

//printing stuff
void printFamilyDb(Family_DB* db);
void printFamily(Family* family_ptr);
void printUsersOfFamily(Family* family_ptr);
void printFullFamilyDetails(Family* familyPtr);

//file ops
Family* insertFamilyFromFile(Family_DB* db,Family* newFamilyTemp);
User* insertUserInFamilyFromFile(Family* familyPtr,User* newUserTemp);
Bool insertExpenseInUserFromFile(Family*familyPtr,User* userPtr,Expense* expense_static_ptr);

#endif
