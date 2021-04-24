/*
 * DIR file contains information about files in IMG file
 */

#ifndef FILE_DIR_H
#define FILE_DIR_H

#include <stdio.h> // FILE
#include <stdlib.h> // malloc
#include <stdint.h> // uint32_t

struct dir_file_t {
    uint32_t position;
    uint16_t size_second_priority;
	uint16_t size_first_priority;
    char name[24];
};

struct dir_file_t *dir_files;

int dir_file_load(const char *filepath);
void dir_file_cleanup();

#endif