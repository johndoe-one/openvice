#include <stdlib.h>

#include "window.h"
#include "file_dir.h"
#include "file_img.h"

int main(int argc, char *argv[]) {
    const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.dir";
    const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.img";

    file_dir_load(dir_filepath);
    file_img_load(img_filepath);

    file_img_export_asset(0);

    if (window_init()) {
        window_loop();
    }
    window_cleanup();

    file_img_cleanup();
    file_dir_cleanup();

    return EXIT_SUCCESS;
}
