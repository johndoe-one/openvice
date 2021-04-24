/*
 * IMG file contains all assets raw data
 */

#ifndef FILE_IMG_H
#define FILE_IMG_H

#include <stdio.h> // FILE
#include <stdlib.h> // malloc

#define IMG_BLOCK_SIZE 2048 // 2 KB

FILE *file_ptr_img;

int img_file_load(const char *filepath);
int img_file_export_asset(unsigned int file_index);
void img_file_cleanup();

#endif
