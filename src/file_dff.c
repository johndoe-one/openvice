#include "file_dff.h"

int file_dff_load(const char *bytes)
{
        return 0;
}

void file_dff_dump(const char *bytes)
{
        struct header section_clump_header;
        struct clump section_clump_data;
        size_t offset_header;

        memcpy(&section_clump_header, bytes, sizeof(struct header));

        offset_header = sizeof(struct header);
        memcpy(&section_clump_data,
                bytes + offset_header, sizeof(struct clump));

        printf("Dump DFF file\n");

        printf("CHUNK_CLUMP header: size = %d\n", 
                section_clump_header.size);
        printf("CHUNK_CLUMP header: type = %d (%#x)\n", 
                section_clump_header.type,
                section_clump_header.type);
        printf("CHUNK_CLUMP header: unknown = %d\n", 
                section_clump_header.unknown);
        printf("CHUNK_CLUMP header: version_number = %d\n", 
                section_clump_header.version_number);

        if (section_clump_header.type == CHUNK_CLUMP)
                printf("CHUNK_CLUMP header: this CHUNK_CLUMP section\n");
        else
                printf("CHUNK_CLUMP header: not CHUNK_CLUMP section\n");

        printf("CHUNK_CLUMP data: object count = %d\n",
                section_clump_data.object_count);

        printf("End dump DFF file\n");
}

void file_dff_cleanup()
{
}
