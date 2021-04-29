/*
 * DFF file contains info about 3D model
 */

#ifndef FILE_DFF_H
#define FILE_DFF_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

enum chunk_type {
        CHUNK_DATA = 1,
        CHUNK_STRING = 2,
        CHUNK_EXTENSION = 3,
        CHUNK_TEXTURE = 6,
        CHUNK_MATERIAL = 7,
        CHUNK_MATERIAL_LIST = 8,
        CHUNK_FRAMELIST = 14,
        CHUNK_GEOMETRY = 15,
        CHUNK_CLUMP = 16,
        CHUNK_ATOMIC = 20,
        CHUNK_GEOMETRY_LIST = 26,
        CHUNK_MATERIAL_SPLIT = 1294,
        CHUNK_FRAME = 39056126
};

struct header {
        uint32_t type;
        uint32_t size;
        uint16_t unknown;
        uint16_t version_number;
};

struct clump {
        uint32_t object_count;
};

int file_dff_load(const char *bytes);
void file_dff_dump(const char *bytes);
void file_dff_cleanup();

#endif
