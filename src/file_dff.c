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

        struct header header_extension;
        struct header header_chunk;

        size_t end;

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

        for (i = 0; i < num_frames; i++) {
                printf("header CHUNK_EXTENSION\n");
                header_extension = read_header(bytes, offset);

                end = *offset;
                end += header_extension.size;

                /* headers extension */
                while (*offset < end) {
                        printf("header ");
                        header_chunk = read_header(bytes, offset);

                        switch (header_chunk.type) {
                        case CHUNK_FRAME:
                                printf("CHUNK_FRAME\n");
                                *offset += header_chunk.size;
                                break;
                        case CHUNK_HANIM_PLG:
                                printf("CHUNK_HANIM_PLG\n");
                                *offset += header_chunk.size;
                                break;
                        default:
                                printf("undefined\n");
                                *offset += header_chunk.size;
                                break;
                        }

                        dump_header(header_chunk, *offset);
                }
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

                memcpy(&geometry_data, bytes + *offset,
                        sizeof(struct geometry_data));
                *offset += sizeof(struct geometry_data);

                printf("triangle_count (face count) = %d\n",
                        geometry_data.triangle_count);
                printf("vertex_count = %d\n", geometry_data.vertex_count);

                 if (head.version_number < 0x34000) {
                        struct light_info light_info;
                        memcpy(&light_info, bytes + *offset, 
                                sizeof(struct light_info));
                        *offset += sizeof(struct light_info);
                }

                if (!geometry_data.has_native_geometry) {
                        if (geometry_data.flags & FLAGS_PRELIT) {
                                struct vertex_color *vertex_colors;
                                size_t sz;

                                sz = geometry_data.vertex_count * sizeof(
                                        struct vertex_color);

                                vertex_colors = (struct vertex_color*)malloc(
                                        sz);
                                memcpy(vertex_colors, bytes + *offset, sz);
                                *offset += sz;

                                free(vertex_colors);
                        }

                        if (geometry_data.flags & FLAGS_TEXTURED) {
                                struct tex_coord *tex_coords;
                                size_t sz;

                                sz = geometry_data.vertex_count * sizeof(
                                        struct tex_coord);

                                tex_coords = (struct tex_coord*)malloc(sz);
                                memcpy(tex_coords, bytes + *offset, sz);

                                *offset += sz;

                                free(tex_coords);
                        }

                        if (geometry_data.flags & FLAGS_TEXTURED2) {
                                size_t sz;

                                sz = geometry_data.num_uvs * geometry_data.vertex_count * sizeof(
                                        struct tex_coord);

                                for (uint32_t j = 0; j < geometry_data.num_uvs; j++) {
                                        struct tex_coord* tex_coords;

                                        tex_coords = (struct tex_coord*)malloc(sz);
                                        memcpy(tex_coords, bytes + *offset, sz);
                                        *offset += sz;

                                        free(tex_coords);
                                }
                        }

                        struct face *faces;
                        size_t sz = geometry_data.triangle_count * sizeof(struct face);

                        faces = (struct face*)malloc(sz);
                        memcpy(faces, bytes + *offset, sz);
                        *offset += sz;

                        free(faces);
                }

                struct sphere sphere;
                memcpy(&sphere, bytes + *offset, sizeof(struct sphere));
                *offset += sizeof(struct sphere);

                if (!geometry_data.has_native_geometry) {
                        struct vertex *vertices;
                        size_t sz;

                        sz = geometry_data.vertex_count * sizeof(struct vertex);
                        vertices = (struct vertex*)malloc(sz);

                        memcpy(vertices, bytes + *offset, sz);
                        *offset += sz;

                        for (int j = 0; j < geometry_data.vertex_count; j++) {
                                printf("%f %f %f \n", vertices[j].x, vertices[j].y, vertices[j].z);
                        }

                        free(vertices);

                        if (geometry_data.flags & FLAGS_NORMALS) {
                                struct vertex* normals;
                                size_t sz;
                                
                                sz = geometry_data.vertex_count * sizeof(struct vertex);
                                normals = (struct vertex*)malloc(sz);

                                memcpy(normals, bytes + *offset, sz);
                                *offset += sz;

                                free(normals);
                        }
                }

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
