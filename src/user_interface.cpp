/**
 * Author : Bihan Sen (senbihan@gmail.com)
 *
 **/

#include <bits/stdc++.h>
#include <unistd.h>
#include "reader.h"
#include "user_interface.h"
#include "writer.h"
#include "operations.h"
#define BUF_SIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

using namespace std;

int argc;
char *file ;
void print_welcome()
{
    printf("| \n|\n| A simple Database Management System using Indexing\n|\n");
    //printf("| \n| This project is open-sourced. To know more please visit \n");
    //printf("| %c[4mwww.github.com/senbihan/csdbms%c[0m \n",27,27);
    printf("| \n| Type 'help' to see the query syntax.\n| Type 'exit' to Exit.\n");
    printf("| \n");
}   

char* read_line()
{
    char *buff = NULL;
    size_t bufsize = 0;
    getline(&buff,&bufsize,stdin);
    return buff;
}

char** split_line(char *buff)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = (char **)malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) ERR_MESG("Allocation Error\n");

    token = strtok(buff, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = (char **)realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) ERR_MESG("Allocation Error\n");
                
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    argc = position;
    return tokens;
}

int print_queries()
{
    printf("\n\n\n");
    printf("CREATE %5c",' ');
    printf(": To create a new database \n");
    printf("\n%14ccreate table <table-name>",' ');  
    printf("\n%14c<Column-1-Name> <type>(i: integer|s : string |r : real|d : date|t : time) ",' ');
    printf("\n%14c<size>, [[for real value enter comma between integral and frac part size, like 5,3 ]]",' ');
    printf("\n%14c<constraint>(p : primary key, n : not null, a : auto_incr)",' ');
    printf("\n%14c<Column-2-Name> ..",' ');
    printf("\n\n\n");
    printf("DESC %7c",' ');
    printf(": To describe the table.");
    printf("\n%14cdesc|describe <table-name>:  to show the schema of the table",' ');
    printf("\n\n\n");
    printf("INSERT %5c",' ');
    printf(": Inserts into database.");
    printf("\n%14cinsert into <table-name> values val1 val2 ... [according to the order of column]",' ');
    printf("\n\n\n");    
    printf("SELECT %5c",' ');
    printf(": select * from <table-name> where col_i val_i ... [Only Equality and based on AND condition]");
    printf("\n\n\n");    
    printf("DELETE %5c",' ');
    printf(": delete from <table-name> where col_i val_i ... [Only Equality and based on AND condition]");
    printf("\n\n\n");
    printf("HELP %7c",' ');
    printf(": 'help' to show this menu");
    printf("\n\n\n");    
    
    return 1;
}

int exec_line(int argc, char **args)
{
    if(strcmp(args[0],"help") == 0 || strcmp(args[0],"HELP") == 0)
        return print_queries();

    if(strcmp(args[0],"exit") == 0 || strcmp(args[0],"EXIT") == 0){
        OK_MESG("Thank You for using CSDBMS!\n");
        //printf("\nContribute at www.github.com/senbihan/csdbms\n\n");
        return 0;
    }

    /** Create Database **/
    if(strcmp(args[0],"create") == 0 && strcmp(args[1],"table") == 0)
    {
        if(argc <= 3 || (argc > 3 && (argc - 3) % 4 != 0)) {
            WARN_MESG("Invalid Syntax. Type 'help' to see all Syntax\n"); 
            return 1;
        }
        file = create_db(argc,args);
        if(file == NULL)
            FAIL_MESG("Database Not Created\n");
    }

    /** Describe Table : shows schema of the table**/
    else if(argc == 2 && (strcmp(args[0],"desc") == 0 || strcmp(args[0],"describe") == 0))
        show_schema(args[1]);
    
    /** Select Data **/
    else if(argc >= 4 && strcmp(args[0],"select") == 0)
        show_data(argc,args);

    /** Insert Data **/
    else if(argc > 3 && strcmp(args[0],"insert") == 0 && strcmp(args[1],"into") == 0){
        if(insert_data(argc,args)){  
            OK_MESG("\n1 row inserted\n");
        }
        else                        
            FAIL_MESG("\nInsertion Failed\n");
    }
    /** Delete Data **/
    else if(argc > 2 && strcmp(args[0],"delete") == 0 && strcmp(args[1],"from") == 0){
        delete_data(argc,args);
    }
    else
        WARN_MESG("\nInvalid Syntax. Type 'help' to see all Syntax\n"); 
    return 1;
}


void start_session()
{
    string inp;
    char *line;
    char **args;
    int stat = 1;
    //file = Malloc(char, 10);
    print_welcome();
    do
    {
        printf(">>> ");
        line = read_line();     
        if(line[0] == '\n') continue;   
        args = split_line(line);
        stat = exec_line(argc,args);
        free(line);
        free(args);
    }while(stat);
    free(file);
}

