#include <stdlib.h>
#include "MemoryFile.h"

#define ALLOCATION_INCREMENT 1024
#define HEXADECIMAL_DIGITS "0123456789ABCDEF"

MemoryFile *MemoryFile_create()
{
    MemoryFile *file = (MemoryFile *)malloc(sizeof(MemoryFile));
    file->start = NULL;
    file->current = NULL;
    file->allocated = 0;
    return file;
}

static void MemoryFile_allocate(MemoryFile *file, unsigned required)
{
    if (file->allocated >= required)
        return;
    
    while (file->allocated < required)
        file->allocated += ALLOCATION_INCREMENT;
    
    int offset = file->current - file->start;
    file->start = (char *)realloc(file->start, file->allocated * 2);
    file->current = file->start + offset;
}

void MemoryFile_write(MemoryFile *file, unsigned char *data, unsigned length)
{
    MemoryFile_allocate(file, (file->current - file->start) / 2 + length + 1);
    for (unsigned i = 0; i < length; i++)
    {
        *file->current++ = HEXADECIMAL_DIGITS[data[i] >> 4];
        *file->current++ = HEXADECIMAL_DIGITS[data[i] & 0x0f];
    }
    *file->current = 0;
}

MemoryFile *MemoryFile_open(char *data)
{
    MemoryFile *file = MemoryFile_create();
    file->start = data;
    file->current = file->start;
    return file;
}

static unsigned char parse_hexadecimal(char digit)
{
    if (digit >= '0' && digit <= '9')
        return digit - '0';
    if (digit >= 'a' && digit <= 'f')
        return digit - 'a' + 10;
    return digit - 'A' + 10;
}

void MemoryFile_read(MemoryFile *file, unsigned char *buffer, unsigned length)
{
    for (unsigned i = 0; i < length; i++)
    {
        if (*file->current == 0)
            return;
        buffer[i] = parse_hexadecimal(*file->current++) << 4;
        buffer[i] += parse_hexadecimal(*file->current++);
    }
}

void MemoryFile_dispose(MemoryFile *file)
{
    if (file->allocated > 0)
        free(file->start);
    free(file);
}
