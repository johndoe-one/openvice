/*
 * Copyright (C) 2021 Misha Samoylov
 */

#include "file_dff.h"

static struct header read_header(const char* bytes, size_t* offset)
{
        struct header head;

        memcpy(&head, bytes + *offset, sizeof(struct header));
        *offset += sizeof(struct header);

        return head;
}

static void dump_header(struct header head, size_t offset)
{
        printf("type = %d (%#x)\n", head.type, head.type);
        printf("size = %d bytes\n", head.size);
        printf("version_number = %#x\n", head.version_number);
        printf("offset = %d\n", (int)offset);
        printf("\n");
}

static struct clump_data read_clump_data(const char* bytes, size_t* offset)
{
        struct clump_data data;

        memcpy(&data, bytes + *offset, sizeof(struct clump_data));
        *offset += sizeof(struct clump_data);

        printf("object_count = %d\n", data.object_count);
        printf("lights_count = %d\n", data.lights_count);
        printf("camera_count = %d\n", data.camera_count);
        printf("\n");

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

        printf("num_frames: %d\n", num_frames);
        printf("\n");

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

                printf("header ");
                head = read_header(bytes, offset);

                switch (head.type) {
                case CHUNK_FRAME:
                        printf("CHUNK_FRAME\n");
                        break;
                case CHUNK_HANIM_PLG:
                        printf("CHUNK_HANIM_PLG\n");
                        break;
                default:
                        printf("undefined\n");
                        break;
                }
                dump_header(head, *offset);

                *offset += head.size;
        }
}

static void read_geometry_list_data(const char* bytes, size_t* offset)
{
        struct header head;
        uint32_t geometry_count;
        struct geometry_data geometry_data;

        size_t temp_offset;
        int i;

        memcpy(&geometry_count, bytes + *offset, sizeof(uint32_t));
        *offset += sizeof(uint32_t);

        printf("geometry_count = %d\n", geometry_count);

        for (i = 0; i < geometry_count; i++) {
                printf("header CHUNK_GEOMETRY\n");
                head = read_header(bytes, offset);

                temp_offset = *offset;

                printf("header CHUNK_STRUCT\n");
                read_header(bytes, offset);

                memcpy(&geometry_data, bytes + *offset, sizeof(struct geometry_data));
                *offset += sizeof(struct geometry_data);

                printf("triangle_count (face count) = %d\n", geometry_data.triangle_count);
                printf("vertex_count = %d\n", geometry_data.vertex_count);

                /* skip light info */
                /* float 4 bytes ambient */
                /* float 4 bytes diffuse */
                /* float 4 bytes specular */
                if (head.version_number < 0x34000)
                        *offset += 4 + 4 + 4;

                /* skip to next CHUNK_GEOMETRY */
                *offset = temp_offset;
                *offset += head.size;
        }
}

static struct atomic_data read_atomic_data(const char *bytes, size_t *offset)
{
        struct atomic_data data;

        memcpy(&data, bytes + *offset, sizeof(data));
        *offset += sizeof(data);

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
        struct clump_data clump_data;
        size_t offset;
        int i;

        offset = 0;

        printf("Dump DFF file\n");

        printf("header CHUNK_CLUMP\n");
        header = read_header(bytes, &offset);

        printf("header CHUNK_STRUCT\n");
        header = read_header(bytes, &offset);

        printf("data CHUNK_STRUCT\n");
        clump_data = read_clump_data(bytes, &offset);

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

        for (i = 0; i < clump_data.object_count; i++) {
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
