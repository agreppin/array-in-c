/* inspired by https://bytesbeneath.com/p/dynamic-arrays-in-c */
#include "array.h"

void *
array_init(size_t item_size, size_t capacity)
{
  size_t size = item_size * capacity + sizeof(Array_Header);
  Array_Header *h = (Array_Header *)malloc(size);

  if (h) {
    h->length = 0;
    h->capacity = capacity;
    h->item_size = item_size;
    ++h;
  }

  return h;
}

#ifndef __cplusplus
void
array_free(void *a)
{
  if (a) {
    Array_Header *h = array_header(a);
    free(h);
  }
}

void *
array_ensure_capacity(void *a, size_t minCapacity)
{
  Array_Header *h = array_header(a);

  if (minCapacity <= h->capacity)
    return a;

  size_t new_capacity = h->capacity * 2;
  while (new_capacity < minCapacity)
    new_capacity *= 2;

  size_t new_size = sizeof(*h) + new_capacity * h->item_size;
  h = (Array_Header *)realloc(h, new_size);
  h->capacity = new_capacity;
  return h ? ++h : h;
}
#endif
