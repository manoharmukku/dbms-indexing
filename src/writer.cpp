/**
 * Author : Bihan Sen (senbihan@gmail.com)
 *
 **/

#include "writer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
using namespace std;

FILE *fptr;
ofstream ofile;
int *data_types;

char* table_to_file_name(char *tab_name)
{
    char ext[4] = ".cs";
    char *filename = Malloc(char,10);
    strncpy(filename,tab_name,strlen(tab_name));
    return strcat(filename,ext);
}   

void create_db_file(char *fname)
{
    if(fptr != NULL)    fclose(fptr);
    fptr = fopen(fname,"wb+");
    if(NULL == fptr) ERR_MESG("writer.cpp : file cannot be opened!")
    fclose(fptr);
}

void write_file_header()
{
    fwrite("CS",sizeof(char),FILE_HEADER_SIZE,fptr);
}

void write_timestamp(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,DATE_TIME_POSITION,SEEK_SET);
    time_t t = time(NULL);
    struct tm tm_inf = *localtime(&t);
    long yy     = tm_inf.tm_year + 1900;
    long mon    = tm_inf.tm_mon + 1;
    long dd     = tm_inf.tm_mday;
    long date   = yy * 10000L + mon * 100L + dd;
    long hh     = tm_inf.tm_hour;
    long mm     = tm_inf.tm_min;
    long ss     = tm_inf.tm_sec;
    LAST_MOD_TIME = ((date * 100L + hh) * 100L + mm ) * 100L + ss; 
    //printf("writing date time = %ld\n",LAST_MOD_TIME);
    fwrite(&LAST_MOD_TIME,DATE_TIME_SIZE,1,fp);
}

void write_no_records(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,NO_REC_POSITION,SEEK_SET);
    fwrite(&NO_RECORDS,RECORD_NO_SIZE,1,fp);
}

void write_no_columns(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,NO_COL_POSITION,SEEK_SET);
    fwrite(&NO_COLUMNS,COLUMN_NO_SIZE,1,fp);
}

void write_table_name()
{
    fwrite(TABLE_NAME,sizeof(char),TABLE_NAME_SIZE,fptr);
}

void write_data_head()
{
    DATA_HEAD = ftell(fptr) + DATA_HEAD_SIZE + DATA_END_SIZE ;
    DATA_HEAD += FIRST_REC_NO_SIZE + LAST_REC_NO_SIZE + TOTAL_REC_SIZE;
    DATA_HEAD += NO_COLUMNS * sizeof(struct Column);
    DATA_HEAD += BUFFER_SIZE;
    fwrite(&DATA_HEAD, DATA_HEAD_SIZE, 1, fptr);
}

void write_data_end(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,DATA_END_POSITION,SEEK_SET);
    DATA_END = D_END;
    fwrite(&DATA_END, DATA_END_SIZE, 1, fp);
}

void write_size_of_records()
{
    fwrite(&RECORD_SIZE,RECORD_SIZE_SIZE,1,fptr);
}

void write_first_rec_no(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,FIRST_REC_NO_POS,SEEK_SET);
    fwrite(&FIRST_REC_NO,FIRST_REC_NO_SIZE,1,fp);
}


void write_last_rec_no(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,LAST_REC_NO_POS,SEEK_SET);
    fwrite(&LAST_REC_NO,LAST_REC_NO_SIZE,1,fp);
}

void write_total_rec(FILE *fp)
{
    assert(fp != NULL);
    fseek(fp,TOTAL_REC_POS,SEEK_SET);
    fwrite(&TOTAL_RECORD,TOTAL_REC_SIZE,1,fp);
}

/**
 * write all columns to the file
 */
void write_columns()
{
    fwrite(col, sizeof(struct Column), NO_COLUMNS , fptr);
}

void write_to_file()
{
    assert(fptr != NULL);
    write_file_header();
    write_timestamp(fptr);
    write_no_records(fptr);
    write_no_columns(fptr);
    write_table_name();
    write_size_of_records();
    write_data_head();
    write_data_end(fptr);
    write_first_rec_no(fptr);
    write_last_rec_no(fptr);
    write_total_rec(fptr);
    write_columns();
}

char get_type(int type, int cons)
{
    char data_type = char(0);
    assert(type >= 1 && type <= 5);
    assert(cons >= 0 && cons <= 3);
    data_type = char(type - 1);
    data_type <<= 3;
    data_type |= char(cons);
    data_type <<= 2;
    return data_type;        
}

void close_file(FILE *fp)
{
    write_timestamp(fp);
    fclose(fp);
}

/**
 * first creation of database
 **/
char *create_db(int argc, char **args)
{
    TABLE_NAME = Malloc(char, TABLE_NAME_SIZE);
    strcpy(TABLE_NAME,args[2]);

    char *filename = table_to_file_name(TABLE_NAME);
    
    create_db_file(filename);

    fptr = fopen(filename,"wb");
    NO_COLUMNS = (argc - 3) / 4;
    NO_RECORDS = 0; // initially 0
    col = Malloc(struct Column, NO_COLUMNS);
    data_types = Malloc(int, NO_COLUMNS);
    CUM_POS = Malloc(int,NO_COLUMNS);
    
    assert(col != NULL && data_types != NULL);
    char sz;
    char *sizeinfo, *consinfo;
    int  type, cons;
    PRIMARY_KEY_COL_NO = -1;
    LAST_REC_NO = 0;
    FIRST_REC_NO = 1;
    TOTAL_RECORD = 0;
    int k = 3;

    sizeinfo = Malloc(char,10);
    consinfo = Malloc(char,10);

    for(int i = 0 ; i < NO_COLUMNS; i++)
    {
        // column name
        strcpy(col[i].col_name,args[k]);
        col[i].index = 1;   // present
        ++k;

        // columne data type
        char ctype = args[k][0];
        switch(ctype){
            case 'i' :      type = INTEGER; break;
            case 'r' :      type = DOUBLE; break;
            case 's' :      type = STRING; break;
            case 'd' :      type = DATE; break;
            case 't' :      type = TIME; break;
            default  :      WARN_MESG("Not a proper data type. See Help\n"); return NULL;
        }
        ++k;

        // data length
        string temp_string;
        int j = 0, i_part, f_part;
        strncpy(sizeinfo,args[k],strlen(args[k]));
        switch(type)
        {
            case INTEGER:   
                sz = char(atoi(sizeinfo));
                assert(sz <= INT_MAX_DIGIT); 
                break;
            case STRING :   
                sz = char(atoi(sizeinfo));
                assert(sz <= STR_MAX_SIZE); 
                break;
            case DOUBLE:
                j = 0; temp_string = "";
                while(sizeinfo[j] != ',')   temp_string += sizeinfo[j++];
                i_part = atoi(temp_string.c_str());
                assert(i_part <= 10);
                sz = char(i_part);
                ++j;
                sz <<= 4;
                temp_string = "";
                while(sizeinfo[j] != '\0')  temp_string += sizeinfo[j++];
                f_part = atoi(temp_string.c_str());
                assert(f_part <= 4);    // upto 4 decimal places
                sz |= char(f_part);
                break;
            default :  break;
        }
        col[i].size = sz;
        ++k;

        // constraint
        //strncpy(consinfo,args[k],strlen(args[k]));
        //for(int j = 0 ; consinfo[j] != '\n'; j++)
        //{
            if(args[k][0] == 'p'){
                if(PRIMARY_KEY_COL_NO == -1)    PRIMARY_KEY_COL_NO = i;
                else                            ERR_MESG("Only 1 Primary Key can be assigned");
                cons = 0;
            }
            else     cons = 1;
        //}   
        ++k;

        // store type
        col[i].data_type = get_type(type, cons);
        // length calculation
        RECORD_SIZE += type == STRING ? sz * TYPE_SIZE[type] : TYPE_SIZE[type];
        CUM_POS[i] = i == 0 ? 0 : CUM_POS[i-1] + (type == STRING ? sz * TYPE_SIZE[type] : TYPE_SIZE[type]);
    
    }

    free(consinfo);
    free(sizeinfo);

    write_to_file();
    close_file(fptr);
    OK_MESG("\nDatabase Created Successfully....\n");
    return TABLE_NAME;
}

