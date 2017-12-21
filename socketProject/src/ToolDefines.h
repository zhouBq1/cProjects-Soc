#ifndef TOOLDEFINES_H_INCLUDED
#define TOOLDEFINES_H_INCLUDED
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define GET_FILE_NAME(file) get_file_name(file)
//strrchr(x,'/')?strrchr(x,'/')+1:x


#define GET_CURRENT_DATE get_current_date()

#define log_warning() {}
#define log_error {}
#define log_trace {}

#define LOG_PREFIX_DEFAULT(file) log_prefix(GET_FILE_NAME(file) ,GET_CURRENT_DATE)
#define u_print( format, args... )  printf( "%s"format,LOG_PREFIX_DEFAULT(GET_FILE_NAME(__FILE__)) ,##args)

static char *get_file_name(file){
    if(file == NULL)
        return "";
    char *tmpFile = file;

    return strrchr(tmpFile,'/')?strrchr(tmpFile,'/')+1:tmpFile;
}

static char *get_current_date(){
    char *rstDate = malloc(sizeof(char) * 20);
    sprintf(rstDate ,"%s-%s",__DATE__,__TIME__);
    return rstDate;
}

static char *log_prefix(const char *file ,const char *date){
    char *rstStr = malloc(sizeof(char) * 100);
    strcpy(rstStr ,date);
    strcat(rstStr ,"--");
    strcat(rstStr ,file);
    strcat(rstStr ,":\t");
    return rstStr;
}

#endif // TOOLDEFINES_H_INCLUDED
