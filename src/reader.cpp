/**
 * Author : Bihan Sen (senbihan@gmail.com)
 *
 **/

#include "metadata.h"
#include "reader.h"
#include <cstdio>
using namespace std;

FILE *frptr;


void open_file(const char *filename)
{
    //if(frptr != NULL)   fclose(frptr);
    frptr = fopen(filename,"rb");
    if(frptr == NULL)   ERR_MESG("reader : File cannot be opened or doesnot exist!\n");
}

//check if the file is actually in the correct format
int check_validity(FILE *fp)
{
    assert(fp != NULL);
    char *dest = Calloc(char,FILE_HEADER_SIZE);
    fread(dest,sizeof(char),FILE_HEADER_SIZE,fp);
    int ret = strcmp("CS",dest);
    //free(dest);
    return ret == 0;
}

void read_record_no()
{
    assert(frptr != NULL);
    fseek(frptr,NO_REC_POSITION,SEEK_SET);
    fread(&NO_RECORDS,RECORD_NO_SIZE,1,frptr);
}

void read_timestamp(FILE *fp)
{
    assert(fp!= NULL);
    fseek(fp,DATE_TIME_POSITION,SEEK_SET);
    fread(&LAST_MOD_TIME,DATE_TIME_SIZE,1,fp);
}

void read_column_no()
{
    assert(frptr != NULL);
    fread(&NO_COLUMNS,COLUMN_NO_SIZE,1,frptr);
}

void read_table_name()
{
    assert(frptr != NULL);
    TABLE_NAME = Malloc(char, TABLE_NAME_SIZE);
    assert(TABLE_NAME != NULL);
    fread(TABLE_NAME,sizeof(char),TABLE_NAME_SIZE,frptr);
}

void read_record_size()
{
    assert(frptr != NULL);
    fread(&RECORD_SIZE,RECORD_SIZE_SIZE,1,frptr);
}

void read_data_head()
{
    assert(frptr != NULL);
    fread(&DATA_HEAD,DATA_HEAD_SIZE,1,frptr);
}

void read_data_end()
{
    assert(frptr != NULL);
    fread(&DATA_END,DATA_END_SIZE,1,frptr);
}

int get_type_int(char type)
{
    return ((112 & type) >> 5) + 1;
}

int get_const_int(char type)
{
    return ((28 & type) >> 2);
}

void read_columns()
{
    fread(col, sizeof(struct Column), NO_COLUMNS , frptr); 
}

void read_first_rec_no()
{
    fread(&FIRST_REC_NO,FIRST_REC_NO_SIZE,1,frptr);
}

void read_last_rec_no()
{
    fread(&LAST_REC_NO,FIRST_REC_NO_SIZE,1,frptr);
}

void read_total_rec()
{
    fread(&TOTAL_RECORD,TOTAL_REC_SIZE,1,frptr);
}

struct datetime *get_date_time()
{
    long tm = LAST_MOD_TIME;
    struct datetime *dt = Malloc(struct datetime,1);
    dt->ss   = tm % 100;
    tm /= 100;
    dt->mm   = tm % 100;
    tm /= 100;
    dt->hh   = tm % 100;
    tm /= 100;
    dt->day   = tm % 100;
    tm /= 100;
    dt->month   = tm % 100;
    tm /= 100;
    dt->year   = tm % 100;
    return dt;
}



void read_from_file(const char *filename)
{
    //printf("Enter filename to be read: \n");
    //scanf("%s",file_name);
    open_file(filename);

    if(!check_validity(frptr))
        ERR_MESG("reader : file format is not valid!");

    read_timestamp(frptr);
    read_record_no();
    //printf("File Details: \n\n");
    //printf("Records : %d\n",NO_RECORDS);
    read_column_no();
    //printf("Columns : %d\n",NO_COLUMNS);
    read_table_name();
    //printf("Table Name : %s\n",TABLE_NAME);
    read_record_size();
    //printf("Each Record Size: %d\n",RECORD_SIZE);
    read_data_head();
    //printf("Data Head : %d\n",DATA_HEAD);
    read_data_end();
    //printf("Data End : %d\n",DATA_END);
    read_first_rec_no();
    //printf("first record no: %d\n",FIRST_REC_NO);
    read_last_rec_no();
    //printf("Last record no: %d\n",LAST_REC_NO);
    read_total_rec();


    col = Malloc(struct Column, NO_COLUMNS);
    assert(col != NULL);
    DATA_TYPES = Malloc(int,NO_COLUMNS);
    assert(DATA_TYPES != NULL);
    
    read_columns();
    PRIMARY_KEY_COL_NO = -1;

    for(int i = 0 ; i < NO_COLUMNS ; i++){
        DATA_TYPES[i] = get_type_int(col[i].data_type);
        //printf("%s \t %d\n",col[i].col_name,DATA_TYPES[i]);
        string s(col[i].col_name);
        COL_NT[s] = DATA_TYPES[i];
        if(get_const_int(col[i].data_type) == 0)    // 0 : primary key
            PRIMARY_KEY_COL_NO = i;
        //col[i].print_col();
    }
    IS_READ = 1;
    OPEN_FILE = Malloc(char,20);
    strncpy(OPEN_FILE,filename,strlen(filename));
    //printf("Reading Complete..... Closing..\n");
    fclose(frptr);
}