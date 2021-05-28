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
        printf("type %d (%#x), size %d bytes, version %#x, offset %d\n", 
                head.type, head.type, head.size, head.version_number,
                (int)offset);
}

static struct clump_data read_clump_data(const char* bytes, size_t* offset)
{
        struct clump_data data;

        memcpy(&data, bytes + *offset, sizeof(struct clump_data));
        *offset += sizeof(struct clump_data);

        printf("object_count %d, lights_count %d, camera_count %d\n",
                data.object_count, data.lights_count, data.camera_count);

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

        printf("num_frames %d\n", num_frames);

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

static void read_tex_coords(struct geometry_data geometry_data, 
        const char* bytes, size_t* offset)
{
        size_t sz;

        if (geometry_data.flags & FLAGS_TEXTURED) {
                struct tex_coord *tex_coords;

                sz = geometry_data.vertex_count * sizeof(
                        struct tex_coord);

                tex_coords = (struct tex_coord*)malloc(sz);
                memcpy(tex_coords, bytes + *offset, sz);

                *offset += sz;

                free(tex_coords);
        }

        if (geometry_data.flags & FLAGS_TEXTURED2) {
                sz = geometry_data.num_uvs * geometry_data.vertex_count *
                        sizeof(struct tex_coord);

                for (uint32_t j = 0; j < geometry_data.num_uvs; j++) {
                        struct tex_coord *tex_coords2;

                        tex_coords2 = (struct tex_coord*)malloc(sz);
                        memcpy(tex_coords2, bytes + *offset, sz);
                        *offset += sz;

                        free(tex_coords2);
                }
        }
}

static void read_vertices_and_normals(struct geometry_data geometry_data,
        const char *bytes, size_t *offset)
{
        if (!geometry_data.has_native_geometry) {
                struct vertex *vertices;
                size_t sz;
                int i = 0;

                sz = geometry_data.vertex_count * sizeof(struct vertex);
                vertices = (struct vertex*)malloc(sz);

                memcpy(vertices, bytes + *offset, sz);
                *offset += sz;

                for (i = 0; i < geometry_data.vertex_count; i++) {
                        printf("v %f %f %f\n", vertices[i].x, vertices[i].y,
                                vertices[i].z);
                }

                free(vertices);

                if (geometry_data.flags & FLAGS_NORMALS) {
                        struct vertex* normals;
                        size_t sz;

                        sz = geometry_data.vertex_count * sizeof(struct vertex);
                        normals = (struct vertex*)malloc(sz);

                        memcpy(normals, bytes + *offset, sz);
                        *offset += sz;

                        for (i = 0; i < geometry_data.vertex_count; i++) {
                                printf("n %f %f %f\n", normals[i].x,
                                        normals[i].y, normals[i].z);
                        }

                        free(normals);
                }
        }
}

static void read_faces(struct geometry_data geometry_data,
        const char *bytes, size_t *offset)
{
        struct face *faces;
        size_t sz = geometry_data.face_count * sizeof(struct face);

        faces = (struct face*)malloc(sz);
        memcpy(faces, bytes + *offset, sz);
        *offset += sz;

        free(faces);
}

static void read_vertex_colors(struct geometry_data geometry_data,
        const char *bytes, size_t *offset)
{
        if (geometry_data.flags & FLAGS_PRELIT) {
                struct vertex_color *vertex_colors;
                size_t sz;

                sz = geometry_data.vertex_count * sizeof(struct vertex_color);

                vertex_colors = (struct vertex_color*)malloc(sz);
                memcpy(vertex_colors, bytes + *offset, sz);
                *offset += sz;

                free(vertex_colors);
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

                if (geometry_data.flags & FLAGS_TEXTURED)
                        geometry_data.num_uvs = 1;

                printf("face_count %d, vertex_count %d\n",
                        geometry_data.face_count,
                        geometry_data.vertex_count);

                 if (head.version_number < 0x34000) {
                        struct light_info light_info;
                        memcpy(&light_info, bytes + *offset, 
                                sizeof(struct light_info));
                        *offset += sizeof(struct light_info);
                }

                if (!geometry_data.has_native_geometry) {
                        read_vertex_colors(geometry_data, bytes, offset);
                        read_tex_coords(geometry_data, bytes, offset);
                        read_faces(geometry_data, bytes, offset);
                }

                struct sphere sphere;
                memcpy(&sphere, bytes + *offset, sizeof(struct sphere));
                *offset += sizeof(struct sphere);

                read_vertices_and_normals(geometry_data, bytes, offset);

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

        printf("frame_index %d, geometry_index %d, "
                "unknown_a = %d, unknown_b = %d\n",
                data.frame_index, data.geometry_index, data.unknown_a,
                data.unknown_b);

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
        
        printf("\n");

        printf("header CHUNK_FRAME_LIST\n");
        header = read_header(bytes, &offset);

        printf("header CHUNK_STRUCT\n");
        header = read_header(bytes, &offset);

        printf("data CHUNK_STRUCT\n");
        read_frame_list_data(bytes, &offset);

        printf("\n");

        printf("header GEOMETRY_LIST\n");
        header = read_header(bytes, &offset);

        printf("header CHUNK_STRUCT\n");
        header = read_header(bytes, &offset);

        printf("data CHUNK_STRUCT\n");
        read_geometry_list_data(bytes, &offset);

        printf("\n");

        for (i = 0; i < clump_data.object_count; i++) {
                printf("header CHUNK_ATOMIC\n");
                header = read_header(bytes, &offset);

                printf("header CHUNK_STRUCT\n");
                header = read_header(bytes, &offset);

                read_atomic_data(bytes, &offset);

                printf("header CHUNK_EXTENSION\n");
                header = read_header(bytes, &offset);
                offset += header.size;

                printf("\n");
        }

        printf("End dump DFF file\n");

        return 0;
}

void file_dff_cleanup()
{
}
