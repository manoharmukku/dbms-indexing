/**
 * Author : Bihan Sen (senbihan@gmail.com)
 *
 **/

#ifndef READER_H
#define READER_H

#include <cstring>
#include "metadata_struct.h"

extern int      IS_READ;
extern int      *DATA_TYPES;
extern char     *OPEN_FILE;

struct datetime{
    int     year;
    int     month;
    int     day;
    int     hh;
    int     mm;
    int     ss;
};

int check_validity();
void read_timestamp(FILE *);
void read_from_file(const char *filename);
struct datetime *get_date_time();

#endif