// zadanie1.c -- Tomáš Gloznek, 24.9.2018 11:15

#include <stdio.h>
#include <string.h>

void **head;

int sub_up(int x, int by)
{
    if(x%by == 0)
        return x/by;
    return x/by + 1;
}

void *memory_alloc(unsigned int size)
{
    void** now = *head;
    void** tmp = head;
    void** result = NULL;
    int i;

    while(now != NULL)
    {
        for(i=1; i<sub_up(size, 8) && *now!=NULL; i++)
        {
            if(now+1 != *now)
                break;
            now = *now;
        }
        if(i == sub_up(size, 8))
        {
            result = *tmp;
            *tmp = *now;
            return result;
        }
        tmp = now;
        now = *now;
    }
    return NULL;
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
    void** now = head;
    void** next = head+1;

    for(int i=1; i<(size/8); i++) {
        *now = next;
        now = *now;
        next++;
    }
    *now = NULL;
}

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
    int x = 32;
    char region[50];
    memory_init(region, 50);
    char* pointer = (char*) memory_alloc(x);
    if (pointer)
        memset(pointer, 120, x);
    else
        printf("Hello NULL\n");
    if (pointer)
        memory_free(pointer);
    printf("Hello World\n");
    return 0;
}