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

int memory_check(void *ptr)
{
    void** now = head;
    while(*now != NULL)
    {
        if(*now == ptr)
            return 0;
        now = *now;
    }
    return 1;
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
    void** now = head;
    void** tmp;
    while(now != NULL)
    {
        if(*now < valid_ptr)
        {
            now = *now;
        }
        else
        {
            tmp = *now;
            *now = valid_ptr;
            while(memory_check(*now+1))

        }
    }
    return 1;
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
    int x = 3;
    char region[50];
    memory_init(region, 50);
    char* pointer = (char*) memory_alloc(x);
    if (pointer)
        memset(pointer, 120, x);
    else
        printf("Hello NULL\n");
    printf("mem check: %d", memory_check(pointer));
    if (pointer)
        memory_free(pointer);
    printf("Hello World\n");
//    printf("ptr size: %d\n", sizeof(uintptr_t));
    return 0;
}