#include "file_img.h"
#include "file_dir.h"

int file_img_load(const char *filepath) {
    FILE *file_ptr_img;
    file_ptr_img = fopen(filepath, "rb");

    if (file_ptr_img == NULL) {
        printf("Cannot open file: %s\n", filepath);

        return 0;
    }

    printf("File IMG was successful loaded\n");

    return 1;
}

void file_img_cleanup() {
    fclose(file_ptr_img);
}

size_t get_img_asset_size(uint16_t size_first_priority, uint16_t size_second_priority) {
    size_t real_size = size_first_priority ? size_first_priority : size_second_priority;
	real_size *= IMG_BLOCK_SIZE;
	return real_size;
}

int file_img_export_asset(unsigned int file_index) {
    size_t asset_size = get_img_asset_size(
        dir_files[file_index].size_first_priority,
        dir_files[file_index].size_second_priority
    );

    // copy asset from IMG to buffer
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

    return 1;
}
