#ifndef INDEX_IO_H
#define INDEX_IO_H

#define ull int
#define datatype int
#define N 4
#define NODE_SIZE sizeof(Node)
#define TRUE true
#define FALSE false 

char* table_name_to_index(char *tablename);
bool indexInsert(datatype key, ull recordOffset, char* fileName);
ull indexFind(datatype key, char* fileName);


#endif