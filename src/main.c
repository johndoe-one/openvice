#include <stdlib.h>

#include "engine.h"
#include "file_dir.h"
#include "file_img.h"

int main(int argc, char *argv[]) {
    const char *dir_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.dir";
    const char *img_filepath = "C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.img";

    dir_file_load(dir_filepath);
    img_file_load(img_filepath);

    img_file_export_asset(0);

    if (window_init()) {
        window_loop();
    }
    window_cleanup();

    img_file_cleanup();
    dir_file_cleanup();

    return EXIT_SUCCESS;
}
