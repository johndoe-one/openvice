/*
 * Copyright (C) 2021 Misha Samoylov
 */

#include "file_dff.h"

static struct header read_header(const char* bytes, size_t* offset)
{
        struct header head;
        memcpy(&head, bytes + *offset, sizeof(struct header));

        *offset += sizeof(struct header);

        printf("type = %d (%#x)\n", head.type, head.type);
        printf("size = %d bytes\n", head.size);
        printf("version_number = %#x\n", head.version_number);
        printf("offset = %d\n", (int)*offset);
        printf("\n");

        return head;
}

static struct clump_data read_clump_data(const char* bytes, size_t* offset)
{
        struct clump_data data;
        memcpy(&data, bytes + *offset, sizeof(struct clump_data));

        printf("object_count = %d\n", data.object_count);
        printf("lights_count = %d\n", data.lights_count);
        printf("camera_count = %d\n", data.camera_count);
        printf("\n");

        *offset += sizeof(struct clump_data);

        return data;
}

static void read_frame_list_data(const char* bytes, size_t *offset)
{
        uint32_t num_frames;
        int i;

        float rotation_matrix[9];
        float position[3];
        uint32_t parent_frame;
        uint32_t align;

        struct header head;

        memcpy(&num_frames, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        printf("num_frames:% d\n", num_frames);

        /* frames in struct */
        for (i = 0; i < num_frames; i++) {
                memcpy(rotation_matrix, bytes + *offset, 9 * sizeof(float));
                *offset += 9 * sizeof(float);
                
                memcpy(position, bytes + *offset, 3 * sizeof(float));
                *offset += 3 * sizeof(float);
                
                memcpy(&parent_frame, bytes + *offset, sizeof(uint32_t));
                *offset += sizeof(uint32_t);

                memcpy(&align, bytes + *offset, sizeof(uint32_t));
                *offset += sizeof(uint32_t);
        }

        /* headers extensions */
        for (i = 0; i < num_frames; i++) {
                printf("header CHUNK_EXTENSION\n");
                read_header(bytes, offset);

                printf("header CHUNK_FRAME or CHUNK_HANIM or undefined\n");
                head = read_header(bytes, offset);
                *offset += head.size;
        }
}

static void read_geometry_list_data(const char* bytes, size_t* offset)
{
        int i;
        uint32_t geometry_count;
        struct header head;

        memcpy(&geometry_count, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        printf("geometry_count = %d\n", geometry_count);

        for (i = 0; i < geometry_count; i++) {
                printf("header CHUNK_GEOMETRY\n");
                head = read_header(bytes, offset);
                *offset += head.size;
        }
}

static struct atomic_data read_atomic_data(const char *bytes, size_t *offset)
{
        struct atomic_data data;

        memcpy(&data.frame_index, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        memcpy(&data.geometry_index, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        memcpy(&data.unknown_a, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        memcpy(&data.unknown_b, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        printf("frame_index = %d\n", data.frame_index);
        printf("geometry_index = %d\n", data.geometry_index);
        printf("unknown_a = %d\n", data.unknown_a);
        printf("unknown_b = %d\n", data.unknown_b);
        printf("\n");

        return data;
}

int file_dff_load(const char *bytes)
{
        struct header header;
        size_t offset;

        int i;

        printf("Dump DFF file\n");

        printf("header CHUNK_CLUMP\n");
        offset = 0;
        header = read_header(bytes, &offset);

        printf("header CHUNK_STRUCT\n");
        header = read_header(bytes, &offset);

        printf("data CHUNK_STRUCT\n");
        struct clump_data cl_data = read_clump_data(bytes, &offset);

        printf("header CHUNK_FRAME_LIST\n");
        header = read_header(bytes, &offset);

        printf("header CHUNK_STRUCT\n");
        header = read_header(bytes, &offset);

        printf("data CHUNK_STRUCT\n");
        read_frame_list_data(bytes, &offset);

        printf("header GEOMETRY_LIST\n");
        header = read_header(bytes, &offset);

        printf("header CHUNK_STRUCT\n");
        header = read_header(bytes, &offset);

        printf("data CHUNK_STRUCT\n");
        read_geometry_list_data(bytes, &offset);

        for (i = 0; i < cl_data.object_count; i++) {
                printf("header CHUNK_ATOMIC\n");
                header = read_header(bytes, &offset);

                printf("header CHUNK_STRUCT\n");
                header = read_header(bytes, &offset);

                read_atomic_data(bytes, &offset);

                printf("header CHUNK_EXTENSION\n");
                header = read_header(bytes, &offset);
                offset += header.size;
        }

        printf("End dump DFF file\n");

        return 0;
}

void file_dff_cleanup()
{
}
