#include "file_img.h"

static FILE *file_ptr_img;

int file_img_load(const char *filepath) {
    file_ptr_img = fopen(filepath, "rb");

    if (file_ptr_img == NULL) {
        printf("Cannot open file: %s\n", filepath);

        return 1;
    }

    printf("File IMG was successful loaded\n");

    return 0;
}

void file_img_cleanup() {
    fclose(file_ptr_img);
}

int file_img_export_asset(unsigned int file_index, struct dir_file_t *dir_files) {
    size_t asset_size = dir_files[file_index].size * IMG_BLOCK_SIZE;

    // copy asset from IMG to buffer
    char *buf = (char*)malloc(asset_size);
    fseek(file_ptr_img, dir_files[file_index].offset * IMG_BLOCK_SIZE, SEEK_SET);
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
