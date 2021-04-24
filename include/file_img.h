/*
 * Loader for file with .IMG, .DIR extension
 */

#ifndef FILE_IMG_H
#define FILE_IMG_H

#include <stdint.h> // uint32_t
#include <stdio.h> // FILE
#include <stdlib.h> // malloc

#define IMG_BLOCK_SIZE 2048 // 2 KB

struct dir_file_t *dir_files;

FILE *file_ptr_img;

int dir_file_load(const char *filepath);
void dir_file_cleanup();

int img_file_load(const char *filepath);
int img_file_export_asset(unsigned int file_index);
void img_file_cleanup();

#endif
