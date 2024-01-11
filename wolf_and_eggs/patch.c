#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>


typedef int (*orig_func_type)(void *ptr);

void free(void **ptr)
{
    orig_func_type orig_free;
    orig_free = (orig_func_type)dlsym(RTLD_NEXT,"free");
    orig_free(*ptr);

    *ptr = NULL;
}