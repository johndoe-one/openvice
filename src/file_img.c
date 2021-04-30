/*
 * Copyright (C) 2021 Misha Samoylov
 */

#include "file_img.h"

static FILE *file_ptr_img;

int file_img_load(const char *filepath)
{
        file_ptr_img = fopen(filepath, "rb");

        if (file_ptr_img == NULL) {
                printf("Cannot open file: %s\n", filepath);
                return 1;
        }

        printf("File IMG was successful loaded\n");
        return 0;
}

void file_img_cleanup()
{
        fclose(file_ptr_img);
}

int file_img_export_asset(const char *filename, uint32_t size, uint32_t offset)
{
        char *buf;
        size_t asset_size;
        FILE *fp_write;

        asset_size = size * IMG_BLOCK_SIZE;

        /* copy asset from IMG to buffer */
        buf = (char*)malloc(asset_size);
        fseek(file_ptr_img, offset * IMG_BLOCK_SIZE, SEEK_SET);
        fread(buf, asset_size, 1, file_ptr_img);

        /* write buffer to file */
        fp_write = fopen(filename, "wb");
        fwrite(buf, asset_size, 1, fp_write);
        fclose(fp_write);

        free(buf);

        printf("File %s from IMG was saved successfully\n", filename);
        return 0;
}

char *file_img_get_asset(uint32_t size, uint32_t offset)
{
        char *buf;
        size_t asset_size;

        asset_size = size * IMG_BLOCK_SIZE;

        /* copy asset from IMG to buffer */
        buf = (char*)malloc(asset_size);
        fseek(file_ptr_img, offset * IMG_BLOCK_SIZE, SEEK_SET);
        fread(buf, asset_size, 1, file_ptr_img);

        return buf;
}

void file_img_asset_cleanup(char *buffer)
{
        free(buffer);
}
