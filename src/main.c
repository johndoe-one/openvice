#include <stdlib.h>

#include "window.h"
#include "file_dir.h"
#include "file_img.h"
#include "file_dff.h"

#define ASSET_INDEX 64 /* wmyri.dff in IMG file */

int main(int argc, char *argv[]) {
        int err;
        char *asset_buffer;
        struct dir_file *dir_files;

        const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City"
                "\\models\\gta3.dir";
        const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City"
                "\\models\\gta3.img";

        /* load DIR file */
        dir_files = file_dir_load(dir_filepath);
        file_dir_dump_asset(ASSET_INDEX, dir_files);

        /* load IMG file */
        file_img_load(img_filepath);

        file_img_export_asset(dir_files[ASSET_INDEX].name,
                dir_files[ASSET_INDEX].size, dir_files[ASSET_INDEX].offset);
        asset_buffer = file_img_get_asset(dir_files[ASSET_INDEX].size,
                dir_files[ASSET_INDEX].offset);

        /* load DFF file from IMG */
        file_dff_load(asset_buffer);
        file_dff_dump(asset_buffer);
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
