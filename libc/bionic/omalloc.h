/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBC_BIONIC_OMALLOC_H_
#define LIBC_BIONIC_OMALLOC_H_

#include <sys/cdefs.h>
#include <stddef.h>

__BEGIN_DECLS
void *o_malloc(size_t size);
void o_free(void *ptr);
void *o_realloc(void *ptr, size_t size);
void *o_calloc(size_t nmemb, size_t size);
int o_posix_memalign(void **memptr, size_t alignment, size_t size);
void *o_memalign(size_t boundary, size_t size);
void *o_valloc(size_t size);
void *o_pvalloc(size_t size);
size_t o_malloc_usable_size(const void *p);
size_t o___malloc_object_size(const void *p);
struct mallinfo o_mallinfo(void);
__END_DECLS

#endif  // LIBC_BIONIC_OMALLOC_H_

