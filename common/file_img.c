#include "file_img.h"

/*
 * DIR file contains information about files in IMG file
 */

struct dir_file_t {
    uint32_t position;
    uint16_t size_second_priority;
	uint16_t size_first_priority;
    char name[24];
};

int dir_file_load(const char *filepath) {
    FILE *fp = fopen(filepath, "r");

    if (fp == NULL) {
        printf("Cannot open file: %s\n", filepath);

        return 1;
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

    return 0;
}

void dir_file_cleanup() {
    free(dir_files);
}

/*
 * IMG file contains all assets raw data
 */

int img_file_load(const char *filepath) {
    FILE *file_ptr_img;
    file_ptr_img = fopen(filepath, "rb");

    if (file_ptr_img == NULL) {
        printf("Cannot open file: %s\n", filepath);

        return 1;
    }

    printf("File IMG was successful loaded\n");

    return 0;
}

void img_file_cleanup() {
    fclose(file_ptr_img);
}

size_t get_img_asset_size(uint8_t size_first_priority, uint8_t size_second_priority) {
    size_t real_size = size_first_priority ? size_first_priority : size_second_priority;
	real_size *= IMG_BLOCK_SIZE;
	return real_size;
}

int img_file_export_file(unsigned int file_index) {
    size_t asset_size = get_img_asset_size(
        dir_files[file_index].size_first_priority,
        dir_files[file_index].size_second_priority
    );

    // copy file from IMG to buffer
    char *buf = (char*)malloc(asset_size);
    fseek(file_ptr_img, dir_files[file_index].position * IMG_BLOCK_SIZE, SEEK_SET);
    fread(buf, asset_size, 1, file_ptr_img);
    
    // write buffer to file
    FILE *fp_write;
    fp_write = fopen(dir_files[file_index].name, "wb");
    fwrite(buf, asset_size, 1, fp_write);
    fclose(fp_write);

    free(buf);

    printf("File %s was saved successfully\n", dir_files[file_index].name);

    return 0;
}
