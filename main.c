//Family Expense Tracker (Using B Trees)
//by- Hrushikesh Musaloj (BT23CSE024)
//usage- run the 'make' command to compile every file, output file is 'project'
//or use 'gcc linked_list.c B_Tree.c user.c family.c expense.c main.c queue.c file_ops.c' to compile, output is 'a.exe'

#include <stdio.h>
#include "user.h"
#include "family.h"
#include "expense.h"
#include "file_ops.h"

//Startup functions
void showLoginScreen(Family_DB* family_db_ptr);
void showSignUpScreen(Family_DB* family_db_ptr);
void showHomeSreen(Family_DB* family_db_ptr,Family* current_family_ptr);
Family* verifyCredentials(Family_DB * family_db_ptr);

//Home screen functions
void AddUserToFamily(Family* currentFamily_ptr);
void AddExpense(Family* current_family_ptr);
void UpdateDeleteFamilyDetails(Family* current_family_ptr);
void UpdateDeleteExpense(Family* current_family_ptr);
void get_total_monthly_expense(Family* current_family_ptr);
void get_categorical_expense(Family* current_family_ptr);
void print_highest_expense_day(Family* current_family_ptr);
void print_individual_expense(Family* current_family_ptr);
void get_expense_in_period(Family* current_family_ptr);
void get_expense_in_range(Family* current_family_ptr);
//update family hepler
void updateFamilyName(Family* current_family_ptr);
void updateFamilyMemberIncome(Family* current_family_ptr);
void deleteMemberFromFamily(Family* current_family_ptr);
//update expense helper
void updateExpenseOfUser(Family* current_family_ptr,User* user);
void deleteExpenseOfUser(Family* current_family_ptr,User* user);

//helper functions
ExpenseCategory chooseExpenseCategory();
User* chooseUserFromFamily(Family* currentFamilyNode);
Expense* chooseExpenseFromUser(User* user);

int main(){
    Family_DB family_db;
    initializeFamilyDb(&family_db);
    inputDataFromFile(&family_db);
    Bool loop=True;
    int choice;
    printf("\tFamily Expense Manager");
    while(loop){
        printf("\n1.Login to family");
        printf("\n2.Sign up new Family");
        printf("\n3.Exit");
        printf("\nEnter choice (1/2/3): ");
        scanf("%d",&choice);
        switch (choice){
            case 1:{
                showLoginScreen(&family_db);
            }
                break;
            case 2:{
                showSignUpScreen(&family_db);
            }
                break;
            //this is only for testing
            case 4:{
                printf("Family Ids in B Tree:\n");
                printBFS(&(family_db.tree));
            }
            default:{
                loop =False;
                break;
            }
        }
    }
    outputDataToFile(&family_db);
    deleteFamilyDb(&family_db);
}

void showLoginScreen(Family_DB* family_db_ptr){
    printf("\tLogin");
    Family * loggedIn_Family_Pointer=verifyCredentials(family_db_ptr);
    if(loggedIn_Family_Pointer!=NULL){
        showHomeSreen(family_db_ptr,loggedIn_Family_Pointer);
    }
    else{
        printf("\nFailed to Login");
    }
}

Family* verifyCredentials(Family_DB * family_db_ptr){
    Family* family_ptr=NULL;
    int enteredFamilyId;
    int enteredUserId;
    printf("\nEnter Family Id: ");
    scanf("%d",&enteredFamilyId);
    printf("Enter user Id: ");
    scanf("%d",&enteredUserId);
    family_ptr=searchFamily(family_db_ptr,enteredFamilyId);
    if(family_ptr==NULL){
        printf("Family not found!\n");
    }
    else{
        User* user_ptr=searchUserInFamily(family_ptr, enteredUserId);
        if(user_ptr==NULL){
            family_ptr=NULL;
        }
    }
    return family_ptr;
}

void showSignUpScreen(Family_DB* family_db_ptr){
    //adding family and one user for the first time
    Bool signedUp=False;
    printf("\tSign up Family");
    //entering family details
    Family newFamily;
    User newUser;
    printf("\nEnter Family Name: ");
    scanf("%s",&newFamily.familyName);
    printf("\tFirst member details");
    printf("\nEnter name: ");
    scanf("%s",&newUser.userName);
    printf("Enter income: ");
    scanf("%f",&newUser.income);
    newUser.userId=0;
    //now creating the nodes dynamically
    Family * newFamilyNode;
    newFamilyNode=insertFamily(family_db_ptr,&newFamily);
    if(newFamilyNode!=NULL){
        User* newUserNode;
        newUserNode=insertUserInFamily(newFamilyNode,&newUser);
        if(newUserNode!=NULL){
            printf("\nFirst user added successfully");
            printf("\nuser id is: %d",newUserNode->userId);
            signedUp=True;
        }
        else{
            //delete the created family here
            printf("Failed to create user!(Deleting family node)\n");
            deleteFamily(family_db_ptr,newFamilyNode->familyId);
        }
    }
    if(signedUp){
        printf("\nFamily signed up successfully");
        printf("\nYour Family Id is: %d\n",newFamilyNode->familyId);
        showHomeSreen(family_db_ptr,newFamilyNode);
    }
    else{
        printf("\nFamily signed up Failed!");
    }
}

void showHomeSreen(Family_DB* family_db_ptr,Family* current_family_ptr){
    Bool loop=True;
    while(loop){
        int choice;
        printf("\n\tHome Menu");
        printf("\n1.Add User\n2.Add Expense\n3.Update/Delete Family Details\n4.Update/Delete Expense\n5.Get Total Family Expense\n6.Get Categorical Expense\n7.Get Highest Expense Day\n8.Get Individual Expense\n9.Get Expense in Period\n10.Get Expense in Range\n11.Exit");
        printf("\nEnter choice (1-11): ");
        scanf("%d",&choice);
        switch(choice){
            case 1 :{
                AddUserToFamily(current_family_ptr);
            }
            break;
            case 2:{
                AddExpense(current_family_ptr);
            }
            break;
            case 3:{
                UpdateDeleteFamilyDetails(current_family_ptr);
                if(current_family_ptr->numUsers<=0){
                    deleteFamily(family_db_ptr,current_family_ptr->familyId);
                    printf("Family Deleted\n");
                    loop=False;
                }
            }
            break;
            case 4:{
                UpdateDeleteExpense(current_family_ptr);
            }
            break;
            case 5:{
                get_total_monthly_expense(current_family_ptr);
            }
            break;
            case 6:{
                get_categorical_expense(current_family_ptr);
            }
            break;
            case 7:{
                print_highest_expense_day(current_family_ptr);
            }
            break;
            case 8:{
                print_individual_expense(current_family_ptr);
            }
            break;
            case 9:{
                get_expense_in_period(current_family_ptr);
            }
            break;
            case 10:{
                get_expense_in_range(current_family_ptr);
            }
            break;
            case 11:{
                loop=False;
            }
            break;
            //The below functions are for testing only!
            case 14:{
                printFullFamilyDetails(current_family_ptr);
            }
            break;
            case 15:{
                printUsersOfFamily(current_family_ptr);
            }
            break;
            default:{
                loop=False;
            }
        }
    }
}

void AddUserToFamily(Family* currentFamily_ptr){
    printf("\n\tAdd user\n");
    if(currentFamily_ptr->numUsers>=4){
        printf("Cannot add more than 4 members\n");
    }
    else{
        User newUserStatic;
        printf("\nEnter name: ");
        scanf("%s",&newUserStatic.userName);
        printf("Enter income: ");
        scanf("%f",&newUserStatic.income);
        newUserStatic.userId=0;
        User* newUserNode;
        newUserNode=insertUserInFamily(currentFamily_ptr,&newUserStatic);
        if(newUserNode!=NULL){
           printf("User added successfully, user id: %d\n",newUserNode->userId);
        }
        else{
            printf("Failed to Add new user!\n");
        }
    }
}

void AddExpense(Family* current_family_ptr){
    printf("\n\tAdd user\n");
    User* chosenUser=chooseUserFromFamily(current_family_ptr);
    if(chosenUser==NULL){
        printf("User id not found\n");
    }
    else{
        Expense newExpense;
        printf("\nEnter amount: ");
        scanf("%f",&(newExpense.amount));
        printf("Enter date(1-10): ");
        scanf("%d",&(newExpense.date));
        newExpense.category=chooseExpenseCategory();
        newExpense.expenseId=0;//initializing it as zero
        newExpense.userId=chosenUser->userId;
        Expense* newAddedExpense=insertExpenseInUser(current_family_ptr,chosenUser,&newExpense);
        if(newAddedExpense==NULL){
            printf("Failed to add expense\n");
        }
        else{
            printf("Expense Added Successfully\n");
        }
    }
}

void UpdateDeleteFamilyDetails(Family* current_family_ptr){
    printf("\n\tUpdate / Delete Family Details");
    int option=0;
    printf("\n1.Update Family Name\n2.Update Family Member Income\n3.Delete Family Member");
    printf("\nEnter option(1,2,3): ");
    scanf("%d", &option);
    switch(option){
        case 1:{
            updateFamilyName(current_family_ptr);
        }
        break;
        case 2:{
            updateFamilyMemberIncome(current_family_ptr);
        }
        break;
        case 3:{
            deleteMemberFromFamily(current_family_ptr);
        }
        break;
        default:{
            printf("\nInvalid option");
        }
    }
    
}

//update/delete family helper functions
void updateFamilyName(Family* current_family_ptr){
    printf("\n\tUpdate Family Name\n");
    printf("Current Family Name: %s\n",current_family_ptr->familyName);
    printf("Enter New Family Name: ");
    scanf("%s",&(current_family_ptr->familyName));
    printf("Family Name updated\n");
}

void updateFamilyMemberIncome(Family* current_family_ptr){
    printf("\n\tUpdate Family Member Income\n");
    User* chosenUser=chooseUserFromFamily(current_family_ptr);
    if(chosenUser==NULL){
        printf("User not found\n");
    }
    else{
        float oldIncome=chosenUser->income;
        float newIncome=0.0;
        printf("User: %s\n",chosenUser->userName);
        printf("Enter new Income: ");
        scanf("%f",&newIncome);
        chosenUser->income=newIncome;
        current_family_ptr->totalIncome=current_family_ptr->totalIncome-oldIncome+newIncome;
        printf("%s's income updated successfully\n",chosenUser->userName);
    }
}

void deleteMemberFromFamily(Family* current_family_ptr){
    printf("\n\tUpdate Family Member Income\n");
    User* chosenUser=chooseUserFromFamily(current_family_ptr);
    if(chosenUser==NULL){
        printf("User not found\n");
    }
    else{
       Bool deleted=removeUserFromFamily(current_family_ptr,chosenUser->userId);
       if(deleted){
        printf("Deleted Family Member\n");
       }
       else{
        printf("Failed To Delete Family Member\n");
       }
    }
}

//Update delete expense
void UpdateDeleteExpense(Family* current_family_ptr){
    printf("\n\tUpdate / Delete Expense\n");
    User* chosenUser=chooseUserFromFamily(current_family_ptr);
    if(chosenUser==NULL){
        printf("User not found\n");
    }
    else{
        int option=0;
        printf("1.Update Expense\n2.Delete Expense\n");
        printf("Choose Option(1,2): ");
        scanf("%d",&option);
        switch(option){
            case 1:{
                updateExpenseOfUser(current_family_ptr,chosenUser);
            }
            break;
            case 2:{
                deleteExpenseOfUser(current_family_ptr,chosenUser);
            }
            break;
            default:{
                printf("Invalid option\n");
            }
        }
    }
}

void updateExpenseOfUser(Family* current_family_ptr,User* user){
    printf("\tUpdate Expense\n");
    Expense* chosenExpense=chooseExpenseFromUser(user);
    if(chosenExpense==NULL){
        printf("Expense not found!\n");
    }
    else{
        float oldExpenseAmount=chosenExpense->amount;
        printf("Current Expense:\n");
        printExpense(chosenExpense);
        printf("Enter updated expense amount: ");
        scanf("%f",&(chosenExpense->amount));
        chosenExpense->category = chooseExpenseCategory();
        printf("Enter expense date: ");
        scanf("%d",&(chosenExpense->date));
        current_family_ptr->totalExpense=current_family_ptr->totalExpense-oldExpenseAmount+chosenExpense->amount;
        printf("Expense Updated Successfully\n");
    }
    
}

void deleteExpenseOfUser(Family* current_family_ptr,User* user){
    printf("\tDelete Expense\n");
    Expense* chosenExpense=chooseExpenseFromUser(user);
    if(chosenExpense==NULL){
        printf("Expense not found!\n");
    }
    else{
        float oldExpenseAmount=chosenExpense->amount;
        Bool deleted=deleteExpenseFromUserExpenses(user,chosenExpense->expenseId);
        if(!deleted){
            printf("Failed to delete Expense\n");
        }
        else{
            current_family_ptr->totalExpense=current_family_ptr->totalExpense-oldExpenseAmount;
            printf("Expense Deleted Successfully\n");
        }
    }
}

void get_total_monthly_expense(Family* current_family_ptr){
    printf("\n\tGet Total Monthly Expense\n");
    float totalIncome=current_family_ptr->totalIncome;
    float totalExpense=current_family_ptr->totalExpense;
    printf("Total Income of Family: %.2f\n",totalIncome);
    printf("Total Expense of Family: %.2f\n",totalExpense);
    if(totalIncome>totalExpense){
        printf("Total Expense is Within total Income by: %.2f\n",totalIncome-totalExpense);
    }
    else if(totalExpense>totalIncome){
        printf("Total Expense exceeds total Income by: %.2f\n",totalExpense-totalIncome);
    }
}

void get_categorical_expense(Family* current_family_ptr){
    printf("\n\tGet Categorical Expense\n");
    ExpenseCategory category=chooseExpenseCategory();
    ShortExpense expensesOfUsers[current_family_ptr->numUsers];
    calculateCategoricalExpense(current_family_ptr,category,expensesOfUsers);
    float totalExpense=0.0;
    for(int i=0;i<current_family_ptr->numUsers;i++){
        totalExpense+=expensesOfUsers[i].totalExpense;
    }
    if(totalExpense>0.00){
        printf("Total Expense in %s : $ %.2f\n",getCategoryName(category),totalExpense);
        for(int i=0;i<current_family_ptr->numUsers;i++){
            if(expensesOfUsers[i].totalExpense>0.00){
                printf("\n$$$$$$$$$$$$$$$$$$$$$$$\n");
                printf("User Id: %d\n",expensesOfUsers[i].userId);
                printf("Expense Amount: $ %.2f\n",expensesOfUsers[i].totalExpense);
                printf("$$$$$$$$$$$$$$$$$$$$$$$\n");
            }
        }
    }
    else{
        printf("No Expenses of '%s'Found",getCategoryName(category));
    }  
}

void print_highest_expense_day(Family* current_family_ptr){
    printf("\n\tExpenses on Days\n");
    ShortExpense expensesOnDays[MAX_DAYS];
    calculateTotalExpenseOnDays(current_family_ptr,expensesOnDays);
    float totalExpenseOnHighestExpenseDay=expensesOnDays[0].totalExpense;
    int highestExpenseDay=expensesOnDays[0].expenseDay;
    if(totalExpenseOnHighestExpenseDay<=0.00){
        printf("No highest expense day (No expenses found)\n");
    }
    else{
        printf("Highest expense is on Day: %d : Total Expense: $ %.2f\n",highestExpenseDay,totalExpenseOnHighestExpenseDay);
        printf("Expenses:\n");
        for(int i=0;i<MAX_DAYS && expensesOnDays[i].totalExpense>0.00;i++){
            printf("Day: %d : Total Expense: $ %.2f\n",expensesOnDays[i].expenseDay,expensesOnDays[i].totalExpense);
        }
    }
}

void print_individual_expense(Family* current_family_ptr){
    printf("\n\tPrint Individual Expense\n");
    User* chosenUser=chooseUserFromFamily(current_family_ptr);
    if(chosenUser==NULL){
        printf("User not found\n");
    }
    else{
        ShortExpense categoricalExpensesOfUser[MAX_CATEGORIES];
        calculateAllCategoricalExpensesOfUser(chosenUser,categoricalExpensesOfUser);
        if(categoricalExpensesOfUser[0].totalExpense<=0.00){
            printf("No expenses of User found\n");
        }
        else{
            float totalExpenseOfUser=0.00;
            for(int i=0;i<MAX_CATEGORIES;i++){
                totalExpenseOfUser+=categoricalExpensesOfUser[i].totalExpense;
            }
            printf("Total Expense Of User: $ %.2f\n",totalExpenseOfUser);
            for(int i=0;i<MAX_CATEGORIES && categoricalExpensesOfUser[i].totalExpense>0.00;i++){
                ExpenseCategory currentCat=categoricalExpensesOfUser[i].category;
                float currentExpense=categoricalExpensesOfUser[i].totalExpense;
                printf("Category: %s : Total Expense: $ %.2f\n",getCategoryName(currentCat),currentExpense);
            }
        }
    }
}

void get_expense_in_period(Family* current_family_ptr){
    printf("\n\tGet Expenses in Period\n");
    int startDate=0;
    int endDate=0;
    printf("Enter start Date (1-10): ");
    scanf("%d",&startDate);
    printf("Enter End Date (1-10): ");
    scanf("%d",&endDate);
    if(startDate>0 && startDate<=10 && endDate>0 && endDate<=10 && startDate<=endDate){
        int numDates=endDate-startDate+1;
        ExpenseLinkedList expensesListInDate[numDates];// 0th index is the start date
        for(int i=0;i<numDates;i++){
            initializeExpenseLinkedList(&(expensesListInDate[i]));
        }
        getAllExpensesInGivenPeriod(current_family_ptr,expensesListInDate,startDate,endDate);
        for(int i=0;i<numDates;i++){
            printf("Date: %d\n",i+startDate);
            if(expensesListInDate[i].size==0){
                printf("No Expenses on this day\n");
            }
            else{
                printExpenseLinkedList(&(expensesListInDate[i]));
            }
            printf("\n");
        }
        for(int i=0;i<numDates;i++){
            deleteExpenseLinkedList(&(expensesListInDate[i]));
        }
        //at the end clear out all the 
    }
    else{
        printf("Invalid start date or end date\n");
    }
}
void get_expense_in_range(Family* current_family_ptr){
    printf("\n\tGet Expenses in ID Range\n");
    User* chosenUser=chooseUserFromFamily(current_family_ptr);
    if(chosenUser==NULL){
        printf("User not found\n");
    }
    else{
        int startID=0;
        int endID=0;
        printf("Enter start ID (>0): ");
        scanf("%d",&startID);
        printf("Enter End ID (>0): ");
        scanf("%d",&endID);
        if(startID>0 && endID>0 && startID<=endID){
            ExpenseLinkedList expensesInRange;
            initializeExpenseLinkedList(&expensesInRange);
            getAllExpensesInIdRange(chosenUser,&expensesInRange,startID,endID);
            if(expensesInRange.size==0){
                printf("No expenses of User found\n");
            }
            else{
                printExpenseLinkedList(&expensesInRange);
            }
            printf("\n");
            deleteExpenseLinkedList(&expensesInRange);
        }
    }
}

//Helper functions

Expense* chooseExpenseFromUser(User* user){
    Expense* chosenExpense=NULL;
    printf("Choose Expense\n");
    printExpensesOfUser(user);
    int expenseId=-1;
    printf("Enter Expense ID: ");
    scanf("%d",&expenseId);
    chosenExpense= searchExpenseInUser(user,expenseId);
    return chosenExpense;
}

User* chooseUserFromFamily(Family* currentFamilyNode){
    User* chosenUser=NULL;
    printf("Choose User\n");
    printUsersOfFamily(currentFamilyNode);
    int userId=-1;
    printf("\nEnter user Id: ");
    scanf("%d",&userId);
    chosenUser=searchUserInFamily(currentFamilyNode,userId);
    return chosenUser;
}

ExpenseCategory chooseExpenseCategory(){
    ExpenseCategory caregoryList[]={RENT,UTILITY,GROCERY,STATIONARY,LEISURE};
    ExpenseCategory category;
    printf("\nChoose Category");
    printf("\n1.Rent\n2.Utility\n3.Grocery\n4.Stationary\n5.Leisure");
    int choice;
    printf("\nEnter choice(1-5): ");
    scanf("%d",&choice);
    if(choice>=1 && choice<=5){
        category=caregoryList[choice-1];
    }
    else{
        printf("\nInvalid choice, category set to Leisure");
        category=LEISURE;
    }
    return category;
}