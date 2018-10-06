// zadanie1.c -- Tomáš Gloznek, 24.9.2018 11:15

#include <stdio.h>
#include <string.h>

void **head;

void *memory_alloc(unsigned int size)
{
    void *next = *head;
    return next;
}

int memory_free(void *valid_ptr)
{
    return 0;
}

int memory_check(void *ptr)
{
    return 0;
}

void memory_init(void *ptr, unsigned int size)
{
    head = ptr;
    *((double**)head) = (double*)head+1;
}

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
    char region[50];
    memory_init(region, 50);
    char* pointer = (char*) memory_alloc(10);
    if (pointer)
        memset(pointer, 8, 10);
    if (pointer)
        memory_free(pointer);
    printf("\nHello World\n");
    printf("size of: %d", (int)sizeof(double));
    return 0;
}