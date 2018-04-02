#ifndef INSERT_H
#define INSERT_H

#include "metadata_struct.h"
#include <variant>
#include <map>
#include <string.h>
#include <unordered_map>

extern int              NO_RECORDS;
extern int              NO_COLUMNS;
extern char*            TABLE_NAME;
extern int              RECORD_SIZE;
extern int              DATA_HEAD;
extern int              DATA_END;
extern struct Column*   col;
extern std::string      types[6] ;//= {"INTEGER", "DOUBLE", "STRING", "DATE", "TIME"};
extern int              TYPE_SIZE[6];
extern int              PRIMARY_KEY_COL_NO;
extern int*             CUM_POS;
extern int              FIRST_REC_NO;
extern int              LAST_REC_NO;
extern int              IS_READ;
extern int              TOTAL_RECORD;
extern char*            OPEN_FILE;
extern long             LAST_MOD_TIME;

extern std::map<std::string,int>   COL_NT;
extern std::unordered_map<std::variant<int,char*>,int> hash_table;

void show_schema(char *);
void build_hash_table(const char *);
bool insert_data(int, char **);
void delete_data(int, char **);
void show_data(int , char** );

#endif // INSERT_H