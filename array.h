// SPDX-FileCopyrightText: 2024 Alain Greppin
//
// SPDX-License-Identifier: BSD-3-Clause

/* inspired by https://bytesbeneath.com/p/dynamic-arrays-in-c */
#pragma once
#include <stdlib.h>

#ifndef __BEGIN_DECLS
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif
#endif

__BEGIN_DECLS

typedef struct {
  unsigned int capacity;
  unsigned int item_size;
  unsigned int length;
  unsigned int padding;
} Array_Header;

#define array(T, capacity) (T *)array_init(sizeof(T), capacity)

extern void *array_init(size_t item_size, size_t capacity);

static inline Array_Header *
array_header(void *a)
{
  return ((Array_Header *)(a)-1);
}

static inline unsigned int
array_capacity(void *a)
{
  return array_header(a)->capacity;
}

static inline unsigned int
array_length(void *a)
{
  return array_header(a)->length;
}

__END_DECLS

#ifdef __cplusplus
template <typename T>
T *
array_ensure_capacity(T *a, size_t minCapacity)
{
  Array_Header *h = array_header(a);

  if (minCapacity <= h->capacity)
    return a;

  size_t new_capacity = h->capacity * 2;
  while (new_capacity < minCapacity)
    new_capacity *= 2;

  size_t new_size = sizeof(*h) + new_capacity * h->item_size;
  Array_Header *new_h = (Array_Header *)malloc(new_size);
  if (!new_h)
    return NULL;

  size_t n = h->length;
  T *new_a = (T *)(new_h + 1);
  while (n--)
    new_a[n] = a[n];

  *new_h = *h;
  new_h->capacity = (unsigned int)new_capacity;
  free(h);
  return new_a;
}

template <typename T, typename U>
void
array_append(T *&a, U v)
{
  Array_Header *h = array_header(a);
  size_t length = h->length;
  a = array_ensure_capacity(a, length + 1);
  a[array_header(a)->length++] = v;
}

template <typename T, typename U>
void
array_append_n(T *&a, U v[], size_t n)
{
  Array_Header *h = array_header(a);
  size_t length = h->length;
  a = array_ensure_capacity(a, length + n);
  for (size_t i = 0, j = length; i < n; ++i)
    a[j++] = v[i];
  array_header(a)->length += (unsigned int)n;
}

template <typename T>
void
array_free(T *a)
{
  if (a) {
    Array_Header *h = array_header(a);
    size_t n = h->length;
    for (size_t i = 0; i < n; ++i)
      a[i].~T();
    free(h);
  }
}

template <typename T>
void
array_remove(T *a, size_t i)
{
  Array_Header *h = array_header(a);
  if (h->length > i) {
    a[i].~T();
    for (size_t j = i, n = --h->length; j < n; ++j)
      a[j] = a[j + 1];
  }
}
#else
extern void *array_ensure_capacity(void *a, size_t item_count);
extern void array_free(void *a);

#define array_append(a, v)                      \
  do {                                          \
    Array_Header *h = array_header(a);          \
    size_t length = h->length;                  \
    (a) = array_ensure_capacity(a, length + 1); \
    (a)[array_header(a)->length++] = (v);       \
  } while (0)

#define array_append_n(a, v, n)                           \
  do {                                                    \
    Array_Header *h = array_header(a);                    \
    size_t length = h->length;                            \
    (a) = array_ensure_capacity(a, length + (size_t)(n)); \
    for (size_t i = 0, j = length; i < n; ++i)            \
      (a)[j++] = (v)[i];                                  \
    array_header(a)->length += (unsigned int)(n);         \
  } while (0)

#define array_remove(a, i)                            \
  do {                                                \
    Array_Header *h = array_header(a);                \
    if (h->length > i) {                              \
      for (size_t j = i, n = --h->length; j < n; ++j) \
        a[j] = a[j + 1];                              \
    }                                                 \
  } while (0)
#endif
