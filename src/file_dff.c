#include "file_dff.h"

int file_dff_load(const char *bytes)
{
        return 0;
}

void file_dff_dump(const char *bytes)
{
        struct header section_clump;
        memcpy(&section_clump, bytes, sizeof(struct header));

        printf("DFF header: size = %d\n", section_clump.size);
        printf("DFF header: type = %d\n", section_clump.type);
        printf("DFF header: type (hex) = %#10x\n", section_clump.type);
        printf("DFF header: unknown = %d\n", section_clump.unknown);
        printf("DFF header: version_number = %d\n", 
                section_clump.version_number);

        if (section_clump.type == CHUNK_CLUMP) {
                printf("DFF header: this is CHUNK_CLUMP section");
        } else {
                printf("DFF header: this is not CHUNK_CLUMP section");
        }
}

void file_dff_cleanup()
{
}
