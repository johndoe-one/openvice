/*
 * IMG file contains all assets raw data
 * Copyright (C) 2021 Misha Samoylov
 */

#ifndef FILE_IMG_H
#define FILE_IMG_H

#include <stdio.h> /* FILE */
#include <stdlib.h> /* malloc */
#include "file_dir.h"

#define IMG_BLOCK_SIZE 2048 /* 2 KB */

int file_img_load(const char *filepath);
int file_img_export_asset(const char *filename, uint32_t size, uint32_t offset);
char *file_img_get_asset(uint32_t size, uint32_t offset);
void file_img_asset_cleanup(char *buffer);
void file_img_cleanup();

#endif
