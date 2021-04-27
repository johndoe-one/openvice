/*
 * DIR file contains information about files in IMG file
 */

#ifndef FILE_DIR_H
#define FILE_DIR_H

#include <stdio.h> /* FILE */
#include <stdlib.h> /* malloc */
#include <stdint.h> /* uint32_t */

struct dir_file {
    uint32_t offset;
    uint32_t size;
    char name[24];
};

struct dir_file *file_dir_load(const char *filepath);
void file_dir_dump(unsigned int file_index, struct dir_file *df);
void file_dir_cleanup(struct dir_file *df);

#endif
