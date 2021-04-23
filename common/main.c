#include <stdint.h> // uint32_t
#include <stdio.h> // FILE
#include <stdlib.h> // malloc

#define DIR_FILE_INDEX 0
#define IMG_BLOCK_SIZE 2048 // 2KB

struct dir_file_t *dir_file;

size_t get_file_size(const char *filepath) {
    FILE *file = NULL;
    file = fopen(filepath, "r");

    if (file == NULL) {
        printf("Cannot open file: %s\n", filepath);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);

    fclose(file);

    return size;
}

/*
 * DIR file contains information about files in IMG file
 */

struct dir_file_t {
    uint32_t position;
    uint16_t size_second_priority;
	uint16_t size_first_priority;
    char name[24];
};

void dir_file_load(const char *filepath) {
    size_t file_size = get_file_size(filepath);
    printf("DIR file size = %u\n", (unsigned int)file_size);

    size_t expected_count = file_size / sizeof(struct dir_file_t);
    printf("DIR: count files = %u\n", (unsigned int)expected_count);

    dir_file = (struct dir_file_t*)malloc(sizeof(struct dir_file_t) * expected_count);

    FILE *fp = fopen(filepath, "r");
    fread(dir_file, sizeof(struct dir_file_t), expected_count, fp);
    fclose(fp);
}

void dir_file_free() {
    free(dir_file);
}

/*
 * IMG file contains all assets raw data
 */

size_t get_img_asset_size(uint8_t size_first_priority, uint8_t size_second_priority) {
    size_t real_size = size_first_priority ? size_first_priority : size_second_priority;
	real_size *= IMG_BLOCK_SIZE;
	return real_size;
}

int save_file_from_img(const char *img_filepath, unsigned int file_index) {
    FILE *fp;
    fp = fopen(img_filepath, "rb");

    if (fp == NULL) {
        printf("Cannot open file: %s\n", img_filepath);

        return 1;
    }

    size_t asset_size = get_img_asset_size(
        dir_file[file_index].size_first_priority,
        dir_file[file_index].size_second_priority
    );

    // copy file from IMG to buffer
    char *buf = (char*)malloc(asset_size);
    fseek(fp, dir_file[file_index].position * IMG_BLOCK_SIZE, SEEK_SET);
    fread(buf, asset_size, 1, fp);
    fclose(fp);

    // write buffer to file
    FILE *fp_write;
    fp_write = fopen(dir_file[file_index].name, "wb");
    fwrite(buf, asset_size, 1, fp_write);
    fclose(fp_write);

    free(buf);

    printf("File was saved successfully\n");

    return 0;
}

int main() {
    const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.dir";
    const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.img";

    dir_file_load(dir_filepath);

    save_file_from_img(img_filepath, DIR_FILE_INDEX);

    dir_file_free();

    return 0;
}
