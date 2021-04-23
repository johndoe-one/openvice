#include "file_img.h"

#define ASSET_FILE_INDEX 0

int main() {
    const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.dir";
    const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.img";

    dir_file_load(dir_filepath);
    img_file_load(img_filepath);

    img_file_export_asset(ASSET_FILE_INDEX);

    img_file_cleanup();
    dir_file_cleanup();

    return 0;
}
