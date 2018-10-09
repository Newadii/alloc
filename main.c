// zadanie1.c -- Tomáš Gloznek, 24.9.2018 11:15

#include <stdio.h>
#include <string.h>

#define u1 unsigned char
#define u2 unsigned short
#define u4 unsigned int
#define u8 unsigned long long

void *head;

u2 ptr_shift(u2 size)
{
    if(size >= 0x7fff)
        return 3;
    if(size >= 0x7f)
        return 2;
    return 1;
}

void *block_header(void *ptr, int size)
{
    if(size > 0xffff)
        return NULL;
    if(size < 0x80)
    {
        *(char *)ptr = (char)(size - 1);
        return (char *)ptr + 1;
    }
    if(size < 0x8000)
    {
        *(u2 *)ptr = (u2)(size - 2) | (u2)0x8000;
        return (char *)ptr + 2;
    }
    *(u2 *)ptr = (u2)(size - 3);
    *((char *)ptr + 2) = 0x00;
    return (char *)ptr + 3;
}

u2 block_size(void *ptr)
{
    if(*((char *)ptr - 1) & 0x80)
        return *((u2 *)ptr - 1) & (u2)0x7fff;
    else if(*((char *)ptr - 1) != 0)
        return (u2)*((char *)ptr - 1);
    else
        return *(u2 *)((char *)(ptr - 3));
}

int can_merge(void **ptr)
{
    if(*ptr == NULL)
        return 0;
    if(*ptr == (char *)ptr + block_size(ptr) + ptr_shift(block_size(*ptr)))
        return 1;
    return 0;
}

void *merge(void **ptr)
{
    void **next = **(void ***)ptr;

    u2 ptr_size = block_size(ptr);
    u2 pptr_size = block_size(*ptr);
    int ptr_shft = ptr_shift(ptr_size);
    int pptr_shft = ptr_shift(pptr_size);

    int new_avbl = ptr_size + pptr_size + ptr_shft + pptr_shft;

    if(new_avbl <= 0xffff)
    {
        ptr = block_header((char *)ptr - ptr_shft, new_avbl);
        *ptr = next;
        return ptr;
    } else
    {
        ptr = block_header(ptr - ptr_shft, 0xfff6);
        *ptr = block_header((char *)ptr + (u2)0xfff3, new_avbl - (u2)0xfff6);
        **(void ***)ptr = next;
        return ptr;
    }
}

void *memory_alloc(unsigned int size)
{
    void **now = &head;
    size += ptr_shift((u2)size);

    while(*now != NULL)
    {
        u2 shift = ptr_shift(block_size(*now));
        u2 avbl = block_size(*now) + shift;
        if(avbl >= size)
        {
            void **next = **(void ***)now;
            void **prev = *now;
            u2 new_avbl = avbl - (u2)size;

            if(new_avbl < 9)
            {
                size = avbl;
                *now = next;
            }
            else
            {
                *now = block_header( ((char *)(*now) + size), new_avbl );
                **(void ***)now = next;
            }
            void **result = block_header(((char *)(prev) - shift), size);
            return result;
        } else if(can_merge(*now))
        {
            *now = merge(*now);
            continue;
        } else
        {
            now = *now;
        }
    }
    return NULL;
}

int memory_free(void *valid_ptr)
{
    if(head == NULL)
    {
        head = valid_ptr;
        *(void **)head = NULL;
        return 0;
    }
    void **now = &head;
    while(*now != NULL)
    {
        if(*now < valid_ptr)
        {
            if(**(void ***)now > valid_ptr || **(void ***)now == NULL)
            {
                *(void **)valid_ptr = **(void ***)now;
                **(void ***)now = valid_ptr;
                return 0;
            } else
                now = *now;
        } else
        {
            *(void **)valid_ptr = *now;
            *now = valid_ptr;
            return 0;
        }
    }
    return 1;
}

int memory_check(void *ptr)
{
    if(head == NULL || ptr < head)
        return 1;
    void **now = head;
    while(now != NULL)
    {
        if(ptr < *now)
        {
            if(ptr < (void *)((char *)now + block_size(now)) )
                return 0;
        }
        now = *now;
    }
    return 1;
}

void memory_init(void *ptr, unsigned int size)
{
    head = ptr;
    void **now = &head;

    while(*now != NULL)
    {
        if(size > 0xffff)
        {
            size -= 0xfff6;
            *now = block_header(*now, 0xfff6);
            now = *now;
            *now = ((char *)now + 0xfff3);
        } else
        {
            *now = block_header(*now, size);
            now = *now;
            *now = NULL;
            return;
        }
    }
}

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
    int x = 10;
    int allc = 100000;
    unsigned char region[allc];
    for(int i = 0; i < allc; i++)
        region[i] = 0x01;
    /*u1 *ptr = region;
    ptr = block_header(ptr, 0x7f + 2);
    x = block_size(ptr);
    int y = ptr_shift(x);*/

    memory_init(region, allc);
    char *pointer = (char *)memory_alloc(9);
    char *pointer2 = (char *)memory_alloc(8);
    memory_free(pointer);
    pointer = (char *)memory_alloc(8);
    can_merge((void *)pointer);

    char *pointer3 = (char *)memory_alloc(8);
    char *pointer4 = (char *)memory_alloc(8);
    char *pointer5 = (char *)memory_alloc(8);
    char *pointer6 = (char *)memory_alloc(8);
    memory_free(pointer2);
    memory_free(pointer3);
    memory_free(pointer5);

    pointer2 = (char *)memory_alloc(8);
    pointer3 = (char *)memory_alloc(8);
    pointer5 = (char *)memory_alloc(8);


    return 0;
}