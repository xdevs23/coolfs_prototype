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
#ifndef _CFS_FS_H_
#define _CFS_FS_H_

#include "superblock.h"

// C001F523 means CoolFS by xdevs23
const char* cfs_fs_header = "\xC0\x01\xF5\x23\x00";

typedef struct _cfs {
    const char* name;
    SUPERBLOCK* superblock;
} CFS;

CFS cfs_create_filesystem(const char* name);
void cfs_write_to_disk(const char* filename, CFS filesystem);
void cfs_free_filesystem(CFS fs);
void cfs_dump_filesystem(CFS fs);

#endif
