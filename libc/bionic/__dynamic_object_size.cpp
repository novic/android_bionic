/*
 * Copyright (C) 2015 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <pthread.h>
#include <stddef.h>
#include <sys/cdefs.h>
#include <unistd.h>

#include "private/libc_logging.h"
#include "pthread_internal.h"

extern "C" size_t __malloc_object_size(const void*);
int __pthread_attr_getstack_main_thread(void** stack_base, size_t* stack_size);

#ifndef __LP64__
extern "C" char __executable_start;
extern "C" char etext;
extern "C" char edata;
extern "C" char end;
#endif

extern const void* __library_region_start;
extern const void* __library_region_end;

static void* main_thread_stack_top = nullptr;

static void main_thread_stack_top_init() {
  if (__predict_false(main_thread_stack_top == nullptr)) {
    void* current_base;
    size_t current_size;
    if (__pthread_attr_getstack_main_thread(&current_base, &current_size)) {
      return;
    }
    main_thread_stack_top = static_cast<char*>(current_base) + current_size;
  }
}

extern "C" size_t __dynamic_object_size(const void* ptr) {
  pthread_internal_t* thread = __get_thread();

  void* stack_base = nullptr;
  void* stack_top = nullptr;
  void* stack_frame = __builtin_frame_address(0);

  if (thread->tid == getpid()) {
    main_thread_stack_top_init();
    if (__predict_true(main_thread_stack_top != nullptr)) {
      stack_base = stack_frame;
      stack_top = main_thread_stack_top;
    }
  } else {
    stack_base = thread->attr.stack_base;
    stack_top = static_cast<char*>(stack_base) + thread->attr.stack_size;
  }

  if (ptr > stack_base && ptr < stack_top) {
    if (__predict_false(ptr < stack_frame)) {
      __libc_fatal("%p is an invalid object address (in unused stack space %p-%p)", ptr, stack_base,
                   stack_frame);
    }
    return static_cast<char*>(stack_top) - static_cast<char*>(const_cast<void*>(ptr));
  }

#ifndef __LP64__
  if (ptr > &__executable_start && ptr < &end) {
    if (ptr < &etext) {
      return &etext - static_cast<char*>(const_cast<void*>(ptr));
    }
    if (ptr < &edata) {
      return &edata - static_cast<char*>(const_cast<void*>(ptr));
    }
    return &end - static_cast<char*>(const_cast<void*>(ptr));
  }
#endif

  if (ptr > __library_region_start && ptr < __library_region_end) {
    return static_cast<char*>(const_cast<void*>(__library_region_end)) -
      static_cast<char*>(const_cast<void*>(ptr));
  }

  return __malloc_object_size(ptr);
}
