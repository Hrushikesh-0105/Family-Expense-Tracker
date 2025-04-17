#ifndef FILE_OPS
#define FILE_OPS

#include "family.h"
#include "user.h"
#include "expense.h"

#define FILENAME "data.txt"


void inputDataFromFile(Family_DB* family_db_ptr);
void outputDataToFile(Family_DB* family_db_ptr);

#endif