/*
 * DFF file contains info about 3D model
 * Copyright (C) 2021 Misha Samoylov
 */

#ifndef FILE_DFF_H
#define FILE_DFF_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

enum chunk_type {
        CHUNK_STRUCT = 1,
        CHUNK_STRING = 2,
        CHUNK_EXTENSION = 3,
        CHUNK_TEXTURE = 6,
        CHUNK_MATERIAL = 7,
        CHUNK_MATERIAL_LIST = 8,
        CHUNK_FRAME_LIST = 14,
        CHUNK_GEOMETRY = 15,
        CHUNK_CLUMP = 16,
        CHUNK_ATOMIC = 20,
        CHUNK_GEOMETRY_LIST = 26,
        CHUNK_HANIM_PLG = 286,
        CHUNK_MATERIAL_SPLIT = 1294,
        CHUNK_FRAME = 39056126
};

struct header {
        uint32_t type;
        uint32_t size;
        uint32_t version_number;
};

struct clump_data {
        uint32_t object_count;
        uint32_t lights_count;
        uint32_t camera_count;
};

struct atomic_data {
        uint32_t frame_index;
        uint32_t geometry_index;
        uint32_t unknown_a;
        uint32_t unknown_b;
};

struct geometry_data {
        uint16_t flags;
        uint8_t num_uvs;
        uint8_t has_native_geometry;
        uint32_t triangle_count;
        uint32_t vertex_count;
        uint32_t morph_target_count;
};

int file_dff_load(const char *bytes);
void file_dff_cleanup();

#endif
