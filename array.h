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
  size_t capacity;
  size_t item_size;
  size_t length;
  size_t padding;
} Array_Header;

#define array(T, capacity) (T *)array_init(sizeof(T), capacity)

void *array_init(size_t item_size, size_t capacity);

static inline Array_Header *
array_header(void *a)
{
  return ((Array_Header *)(a)-1);
}

static inline size_t
array_capacity(void *a)
{
  return array_header(a)->capacity;
}

static inline size_t
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

  new_h->capacity = new_capacity;
  new_h->length = h->length;
  new_h->item_size = h->item_size;
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
  array_header(a)->length += n;
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
void *array_ensure_capacity(void *a, size_t item_count);
void array_free(void *a);

#define array_append(a, v)                      \
  do {                                          \
    Array_Header *h = array_header(a);          \
    size_t length = h->length;                  \
    (a) = array_ensure_capacity(a, length + 1); \
    (a)[array_header(a)->length++] = (v);       \
  } while (0)

#define array_append_n(a, v, n)                 \
  do {                                          \
    Array_Header *h = array_header(a);          \
    size_t length = h->length;                  \
    (a) = array_ensure_capacity(a, length + n); \
    for (size_t i = 0, j = length; i < n; ++i)  \
      (a)[j++] = (v)[i];                        \
    array_header(a)->length += n;               \
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
