/*
 * Copyright (C) 2021 Misha Samoylov
 */

#include "file_dir.h"

struct dir_file *file_dir_load(const char *filepath)
{
        struct dir_file *dir_files = NULL;
        FILE *fp = fopen(filepath, "r");
        size_t file_size;
        size_t assets_count;

        if (fp == NULL) {
                printf("Cannot open file: %s\n", filepath);
                return NULL;
        }

        /* get file size */
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);

        /* create array assets into variable array dir_files */
        assets_count = file_size / sizeof(struct dir_file);
        dir_files = (struct dir_file*)malloc(
                sizeof(struct dir_file) * assets_count);

        /* copy assets with them info to variable array dir_file */
        fseek(fp, 0, SEEK_SET);
        fread(dir_files, sizeof(struct dir_file), assets_count, fp);
        fclose(fp);

        printf("File DIR was successful loaded\n");

        return dir_files;
}

void file_dir_dump_asset(unsigned int file_index, struct dir_file *df)
{
        printf("Dump DIR file asset\n");
        printf("File name = %s\n", df[file_index].name);
        printf("File offset = %d\n", df[file_index].offset);
        printf("File size = %d\n", df[file_index].size);
        printf("End dump DIR file\n");
}

void file_dir_cleanup(struct dir_file *df)
{
        free(df);
}
