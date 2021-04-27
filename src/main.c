#include <stdlib.h>

#include "window.h"
#include "file_dir.h"
#include "file_img.h"
#include "file_dff.h"

#define ASSET_INDEX 64 /* wmyri.dff in IMG file */

int main(int argc, char *argv[]) {
        int err;
        const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City"
                "\\models\\gta3.dir";
        const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City"
                "\\models\\gta3.img";

        struct dir_file *dir_files = file_dir_load(dir_filepath);
        file_dir_dump(ASSET_INDEX, dir_files);
        file_img_load(img_filepath);

        char *asset_buffer = file_img_get_asset(ASSET_INDEX, dir_files);
        file_dff_load(asset_buffer);
        file_dff_cleanup();
        file_img_asset_cleanup(asset_buffer);

        err = window_init();
        if (!err) {
                window_loop();
        }
        window_cleanup();

        file_img_cleanup();
        file_dir_cleanup(dir_files);

        return EXIT_SUCCESS;
}
