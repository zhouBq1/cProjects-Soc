#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED
#define FREE_IF_NOT_NONE(dst) (dst)==NULL ?NULL:free(dst);
#include "ToolDefines.h"
struct ArrayPrivate;
typedef struct _myArray{
    int length;
    void *content;


    void *(*addItem)(void * ,unsigned);
    void *(*removeItem)(void *);
    void *(*itemAtIndex)(unsigned);

    //private properties
    struct ArrayPrivate *priv;
} Array;


//public function for initializing or deleting
Array *newArray(void);
void deleteArray(Array *dArray);
#endif // ARRAY_H_INCLUDED
