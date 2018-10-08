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
    if(size >= 0x8000)
        return 3;
    if(size >= 0x80)
        return 2;
    return 1;
}

void *block_header(void *ptr, int size)
{
    if(size > 0xffff)
        return NULL;
    if(size < 0x81)
    {
        *(char *)ptr = (char)(size - 1);
        return (char *)ptr + 1;
    }
    if(size < 0x8002)
    {
        *(u2 *)ptr = (u2)(size - 2) | (u2)0x8000;
        return (char *)ptr + 2;
    }
    *(u2 *)ptr = (u2)(size - 3);
    *((char *)ptr + 2) = 0x00;
    return (char *)ptr + 3;
}

u2 block_size(void* ptr)
{
    if(*((char *) ptr - 1) & 0x80)
        return *((u2 *) ptr - 1) & (u2) 0x7fff;
    else if(*((char *) ptr - 1) != 0)
        return (u2) *((char *) ptr - 1);
    else
        return *(u2 *)((char *)(ptr - 3));
}

int memory_check(void *ptr)
{
    if(head == NULL)
        return 1;
    void **now = head;
    void **tmp = NULL;
    while(now != NULL)
    {
        if(ptr < (void *)now)
        {
            if(tmp == NULL)
                return 1;
            if(ptr > (void *)((char *) tmp + block_size(tmp)))
                return 1;
        }
        tmp = now;
        now = *now;
    }
    return 0;
}

void *memory_alloc2(unsigned int size)
{
    void **now = &head;
    u2 avbl = block_size(*now);

    while(*now != NULL)
    {
        if(avbl >= size)
        {
            void **next = **(void ***)now;
            u2 shift = ptr_shift(avbl);
            void **result = block_header(((char *)now - shift), size);
            u2 new_avbl = avbl + shift - (u2)size - ptr_shift((u2)size);
            *now = block_header(((char *)result + size), new_avbl);
            **(void ***)now = next;
        }
    }

}

void *memory_alloc(unsigned int size)
{
    if(head == NULL)
        return NULL;
    void **tmp = head;
    void **now = head;
    void **result;
    void **next;
    u2 avbl = block_size(now);
    u2 avbl_new;
    while(now != NULL)
    {
        //todo prepojenie blokov pre vacsiu avbl velkost
//        if(*((char *) now - 1) & 0x80)
//            avbl = *((u2 *) now - 1) & (u2) 0x7fff;
//        else
//            avbl = (u2) *((char *) now - 1);

        if(avbl >= size)
        {
            next = *now;
            if(size < 0x80)
            {
                avbl_new = avbl - (u2)size + ((avbl >= 0x80) ? (u2)1 : (u2)0);
                //todo ak avbl-size nie je dostatocne na vytvorenie medzi bloku
                if(avbl_new < 9)
                {
                    size = avbl;
                    if(tmp == head)
                        head = next;
                    tmp = head;
                } else
                {
                    if(avbl_new < 0x80)
                    {
                        if(tmp == head)
                            head = (void **) ((char *) now + size + ((avbl >= 0x80) ? (u2)0 : (u2)1));
                        tmp = head;
                        *((char *)tmp - 1) = (char)avbl_new;
                    } else
                    {
                        if(tmp == head)
                            head = (void **) ((char *) now + size + 1);
                        tmp = head;
                        *((u2 *)tmp - 1) = avbl_new | (u2)0x8000;
                    }
                    *tmp = next;
                }

                if(avbl >= 0x80)
                {
                    *((char *) now - 2) = (char) size;
                    result = (void **) ((char *) now - 1);
                } else
                {
                    *((char *) now - 1) = (char) size;
                    result = now;
                }

            } else
            {
                avbl_new = avbl - (u2)size;
                //todo ak avbl-size nie je dostatocne na vytvorenie medzi bloku
                if(avbl_new < 9)
                {
                    size = avbl;
                    if(tmp == head)
                        head = next;
                    tmp = head;
                } else
                {
                    if(avbl_new < 0x80)
                    {
                        if(tmp == head)
                            head = (void **) ((char *) now + size + 1);
                        tmp = head;
                        *((char *)tmp - 1) = (char)avbl_new;
                    } else
                    {
                        if(head == tmp)
                            head = (void **) ((char *) now + size + 2);
                        tmp = head;
                        *((u2 *)tmp - 1) = avbl_new | (u2)0x8000;
                    }
                    *tmp = next;
                }

                *((u2 *) now - 1) = (u2) size | (u2) 0x8000;
                result = now;

            }
            return result;
        } else
        {
            u2 blk_sz = block_size(*now);
            u2 add = (blk_sz > 0x7f ? (u2)2 : (u2)1);
            if(*now != NULL && ((char *)(*now) - (char *)now) == (avbl + add) )
            {
                avbl += (blk_sz + add);
                *now = **(void ***)now;
                continue;
            }
            now = *now;
            avbl = block_size(now);
        }
    }
    return NULL;
}

int memory_free(void *valid_ptr)
{
    if(head == NULL)
    {
        head = valid_ptr;
        return 0;
    }
    void **now = head;
    while(now != 0)
    {
        if(now < (void **)valid_ptr)
        {
            if(*now > valid_ptr || *now == NULL)
            {
                *(void **)valid_ptr = *now;
                *now = valid_ptr;
                return 0;
            } else
                now = *now;
        } else
        {
            *(void **)valid_ptr = now;
            head = valid_ptr;
            return 0;
        }
    }

    return 1;
}

void memory_init(void *ptr, unsigned int size)
{
    head = ptr;
    void **now = &head;

    u2 debug;

    while(*now != NULL)
    {
        if(size > 0xffff)
        {
            size -= 0xfff6;
            *now = block_header(*now, 0xfff6);
            now = *now;
            debug = block_size(now);
            *now = ((char *)now + 0xfff6);
        } else
        {
            *now = block_header(*now, size);
            now = *now;
            debug = block_size(now);
            *now = NULL;
            return;
        }
    }
}

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
    int x = 10;
    int allc = 50;
    unsigned char region[100000];
    for(int i = 0; i < allc; i++)
        region[i] = 0x99;

    memory_init(region, 0x12);
    char *pointer = (char *) memory_alloc(8);
    char *pointer2 = (char *) memory_alloc(10);
    char *pointer3 = (char *) memory_alloc(24);
    char *pointer4 = (char *) memory_alloc(13);

    if(pointer)
        memset(pointer, 120, 8);
    if(pointer2)
        memset(pointer2, 120, 10);
    if(pointer3)
        memset(pointer3, 120, 24);
    if(pointer4)
        memset(pointer4, 120, 13);
    else
        printf("Hello NULL\n");

    printf("mem check: %d\n", memory_check(pointer));
//    if(pointer)
//        memory_free(pointer);
    return 0;
}