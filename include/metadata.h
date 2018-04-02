/**
 * Author : Bihan Sen (senbihan@gmail.com)
 *
 **/

#ifndef METADATA_H
#define METADATA_H

#include "metadata_struct.h"
#include <unordered_map>
#include <map>
#include <cstring>  
#include <utility>
#include <variant>
#include <cstdio>
#include <iostream>

int             NO_RECORDS;         /* No of Records logically present */
int             NO_COLUMNS;         /* No of columns */
char*           TABLE_NAME;
int             RECORD_SIZE;
int             DATA_HEAD;
int             DATA_END;
struct Column   *col;
int             *DATA_TYPES;
std::string     types[6] = {"", "INTEGER", "DOUBLE", "STRING", "DATE", "TIME"};
int             TYPE_SIZE[6]   = {0, sizeof(int), sizeof(long), sizeof(char), 0 , 0};
int             PRIMARY_KEY_COL_NO;
int             *CUM_POS;
int             FIRST_REC_NO;
int             LAST_REC_NO;
int             TOTAL_RECORD;
int             IS_READ;
char*           OPEN_FILE;
long            LAST_MOD_TIME;

std::unordered_map<std::variant<int,char*>,int> hash_table;
std::map<std::string,int> COL_NT;


/* Constraint */




#endif // METADATA_H