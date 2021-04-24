#include "file_dir.h"

int file_dir_load(const char *filepath) {
    FILE *fp = fopen(filepath, "r");

    if (fp == NULL) {
        printf("Cannot open file: %s\n", filepath);

        return 0;
    }

    // get file size
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);

    // create array assets into variable array dir_files
    size_t assets_count = file_size / sizeof(struct dir_file_t);
    dir_files = (struct dir_file_t*)malloc(sizeof(struct dir_file_t) * assets_count);  

    // copy assets with them info to variable array dir_file
    fseek(fp, 0, SEEK_SET);
    fread(dir_files, sizeof(struct dir_file_t), assets_count, fp);
    fclose(fp);

    printf("File DIR was successful loaded\n");

    return 1;
}

void file_dir_cleanup() {
    free(dir_files);
}