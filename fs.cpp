/*
 * MIT License
 * 
 * Copyright (C) 2017 Simao Gomes Viana
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <bsd/string.h>
#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#include "fs.h"
#include "superblock.h"
#include "log.h"

CFS cfs_create_filesystem(const char* name) {
    LOG("Creating new instance of CFS");
    CFS cfs = CFS();
    LOGF(" Setting name to \"%s\"", name);
    uint64_t namelen = strlen(name);
    char* cfsname = (char*) malloc(namelen + 1);
    cfsname[namelen] = 0;
    memcpy(cfsname, name, namelen);
    cfs.name = (const char*) cfsname;
    LOGF("  Name set to \"%s\"", cfs.name);
    LOG(" Creating new superblock");
    SUPERBLOCK* sb = cfs_create_superblock();
    LOG(" Creating inode table");
    sb->inode_sizes_len = 4096;
    sb->inode_sizes = (uint64_t*) malloc(sb->inode_sizes_len * sizeof(uint64_t));
    memset(sb->inode_sizes, 0, sb->inode_sizes_len);
    LOG(" Associating superblock to this CFS");
    cfs.superblock = sb;
    LOG(" Created CFS");
    return cfs;
}

void cfs_write_to_disk(const char* filename, CFS filesystem) {
    cfs_dump_filesystem(filesystem);
    LOGF("Writing to file %s...", filename);
    FILE* file = fopen(filename, "wb");

    LOG(" Writing header");
    fwrite(cfs_fs_header, strlen(cfs_fs_header), 1, file);

    LOG(" Writing fs name");
    uint64_t fsname_len = strlen(filesystem.name);

#ifdef BIG_E
    LOG(" Note: Big endian detected. Swapping to little endian.");
    uint64_t fsname_len_swapped = __builtin_bswap64(fsname_len);
    void* fsname_len_ptr = &fsname_len_swapped;
#else
    LOG(" Note: Little endian detected. No swap necessary.");
    void* fsname_len_ptr = &fsname_len;
#endif

    LOG("  Writing...");
    fwrite(fsname_len_ptr, sizeof(uint64_t), 1, file);
    fwrite(filesystem.name, fsname_len, 1, file);

    LOG(" Writing inode tables");
#ifdef LITTLE_E
    fwrite(&filesystem.superblock->inode_sizes_len, sizeof(uint64_t), 1, file);
#else
    uint64_t inode_sizes_len_swapped =
        __builtin_bswap64(filesystem.superblock->inode_sizes_len);
    fwrite(&inode_sizes_len_swapped,
            sizeof(uint64_t), 1, file);
#endif

    LOG(" Writing superblock");
    fwrite(filesystem.superblock->inode_sizes,
            filesystem.superblock->inode_sizes_len * sizeof(uint64_t), 1, file);

    LOG(" Finalizing");
    fflush(file);
    fclose(file);

    LOG("Done.");
}

CFS cfs_read_from_disk(const char* filename) {
    LOGF("Reading from file %s...", filename);

    FILE* file = fopen(filename, "rd");

    char header[CFS_FS_HEADER_LEN];

    LOG(" Checking header");
    fseek(file, 0, SEEK_SET);
    fread(&header, CFS_FS_HEADER_LEN, 1, file);
    if (strncmp(header, cfs_fs_header, CFS_FS_HEADER_LEN)) {
        LOGF("  Filesystem header/magic mismatch!");
        exit(1);
    }

    LOG(" Reading filesystem name")
    uint64_t fsname_len = 0;
    fread(&fsname_len, sizeof(uint64_t), 1, file);
#ifdef BIG_E
    fsname_len = __builtin_bswap64(fsname_len);
#endif

    LOGF("  File system name length: %lu", fsname_len);

    char fsname[fsname_len + 1];
    fsname[fsname_len] = 0;
    fread(&fsname, fsname_len, 1, file);
    LOGF("  Filesystem name: %s", fsname);

    CFS cfs = cfs_create_filesystem(fsname);

    LOG(" Reading inode tables");
#ifdef LITTLE_E
    fread(&cfs.superblock->inode_sizes_len, sizeof(uint64_t), 1, file);
#else
    uint64_t inode_sizes_len_swapped = 0;
    fread(&inode_sizes_len_swapped, sizeof(uint64_t), 1, file);
    cfs.superblock->inode_sizes_len =
        __builtin_bswap64(inode_sizes_len_swapped);
#endif

    LOG(" Reading superblock");
    fread(cfs.superblock->inode_sizes,
            cfs.superblock->inode_sizes_len * sizeof(uint64_t), 1, file);

#ifdef BIG_E
    LOG(" Reversing byte order of inode table sizes");
    for (uint64_t i = 0; i < cfs.superblock->inode_sizes_len; i++) {
        cfs.superblock->inode_sizes[i] =
            __builtin_bswap64(cfs.superblock->inode_sizes[i]);
    }
#endif

    fclose(file);

    LOGF("Done.");
    return cfs_dump_filesystem(cfs);
}

void cfs_free_filesystem(CFS fs) {
    LOG(" Freeing fs name");
    free((void*)fs.name);
    LOG(" Freeing superblock");
    cfs_free_superblock(fs.superblock);
}

CFS cfs_dump_filesystem(CFS fs) {
    LOG("Cool File System")
    LOGF("    Name: %s", fs.name);
    LOG("    Superblock information:");
    LOGF("      Inodes table size: %lu", fs.superblock->inode_sizes_len);
    return fs;
}
