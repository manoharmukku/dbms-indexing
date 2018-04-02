/**
 * Author : Bihan Sen (senbihan@gmail.com)
 *
 **/

#ifndef WRITER_H
#define WRITER_H

#include "metadata_struct.h"
#include <map>
#include <string.h>

extern int              NO_RECORDS;
extern int              NO_COLUMNS;
extern char*            TABLE_NAME;
extern int              RECORD_SIZE;
extern int              DATA_HEAD;
extern int              DATA_END;
extern struct Column*   col;
extern int*             DATA_TYPES;
extern int              TYPE_SIZE[6]  ; //= {0, sizeof(int), sizeof(double), sizeof(char), 0 , 0};
extern int              PRIMARY_KEY_COL_NO;
extern int              *CUM_POS;
extern int              FIRST_REC_NO;
extern int              LAST_REC_NO;
extern int              TOTAL_RECORD;
extern long             LAST_MOD_TIME;
extern std::map<std::string,int> COL_NT;

char *table_to_file_name(char *);
void write_timestamp(FILE *);
void write_no_records(FILE *);
void write_no_columns(FILE *);
void write_data_end(FILE *);
void write_first_rec_no(FILE *);
void write_last_rec_no(FILE *);
void write_total_rec(FILE *);    
void write_to_file();
void close_file(FILE *);
char* create_db(int, char **);


#endif //