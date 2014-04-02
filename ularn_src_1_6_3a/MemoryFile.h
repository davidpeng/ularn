#ifndef __MEMORYFILE_H
#define __MEMORYFILE_H

typedef struct
{
    char *start;
    char *current;
    unsigned allocated;
} MemoryFile;

MemoryFile *MemoryFile_create();
void MemoryFile_write(MemoryFile *file, unsigned char *data, unsigned length);
MemoryFile *MemoryFile_open(char *data);
void MemoryFile_read(MemoryFile *file, unsigned char *buffer, unsigned length);
void MemoryFile_dispose(MemoryFile *file);

#endif
