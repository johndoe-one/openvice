#include <stdlib.h>

#include "window.h"
#include "file_dir.h"
#include "file_img.h"

int main(int argc, char *argv[]) {
        int err;
        const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City"
                "\\models\\gta3.dir";
        const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City"
                "\\models\\gta3.img";

        struct dir_file *dir_files = file_dir_load(dir_filepath);
        file_img_load(img_filepath);

        file_img_export_asset(0, dir_files);

        err = window_init();
        if (!err) {
                window_loop();
        }
        window_cleanup();

        file_img_cleanup();
        file_dir_cleanup(dir_files);

        return EXIT_SUCCESS;
}
