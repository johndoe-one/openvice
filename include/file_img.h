/*
 * IMG file contains all assets raw data
 */

#ifndef FILE_IMG_H
#define FILE_IMG_H

#include <stdio.h> /* FILE */
#include <stdlib.h> /* malloc */
#include "file_dir.h"

#define IMG_BLOCK_SIZE 2048 /* 2 KB */

int file_img_load(const char *filepath);
int file_img_export_asset(unsigned int file_index, struct dir_file *df);
void file_img_cleanup();

#endif
