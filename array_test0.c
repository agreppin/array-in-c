// https://bytesbeneath.com/p/dynamic-arrays-in-c
#include "unused.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ALLOCATOR
typedef struct {
  void *(*alloc)(size_t bytes, void *context);
  void (*free)(size_t bytes, void *ptr, void *context);
  void *context;
} Allocator;

void *
my_alloc(size_t bytes, void *context)
{
  (void)context;
  return malloc(bytes);
}

void
my_free(size_t bytes, void *ptr, void *context)
{
  (void)bytes;
  (void)context;
  free(ptr);
}

Allocator my_allocator = {my_alloc, my_free, 0};

// ARRAY
typedef struct {
  size_t length;
  size_t capacity;
  size_t padding_or_something; // prefer 16-byte alignment
  Allocator *allocator;
} Array_Header;

#define ARRAY_INITIAL_CAPACITY 16

#define array(T, a) array_init(sizeof(T), ARRAY_INITIAL_CAPACITY, a)

void *
array_init(size_t item_size, size_t capacity, Allocator *a)
{
  void *ptr = 0;
  size_t size = item_size * capacity + sizeof(Array_Header);
  Array_Header *h = a->alloc(size, a->context);
  fprintf(stderr, "ini_size=%zu h=%p\n", size, (void *)h);

  if (h) {
    h->capacity = capacity;
    h->length = 0;
    h->allocator = a;
    ptr = h + 1;
  }

  return ptr;
}

#define array_free(a)                                   \
  {                                                     \
    Array_Header *h = array_header(a);                  \
    size_t item_size = sizeof(*a);                      \
    size_t size = h->capacity * item_size;              \
    h->allocator->free(size, h, h->allocator->context); \
  }

/* array_append: please replace sizeof(v) by sizeof(*a) */
#define array_append(a, v)                       \
  ((a) = array_ensure_capacity(a, 1, sizeof(v)), \
   (a)[array_header(a)->length] = (v), &(a)[array_header(a)->length++])

#define array_header(a) ((Array_Header *)(a)-1)
#define array_length(a) (array_header(a)->length)
#define array_capacity(a) (array_header(a)->capacity)

#define array_remove(a, i)             \
  do {                                 \
    Array_Header *h = array_header(a); \
    if (i == h->length - 1) {          \
      h->length -= 1;                  \
    }                                  \
    else if (h->length > 1) {          \
      void *ptr = &a[i];               \
      void *last = &a[h->length - 1];  \
      h->length -= 1;                  \
      memcpy(ptr, last, sizeof(*a));   \
    }                                  \
  } while (0);

#define array_append_n(a, v, n)                        \
  do {                                                 \
    Array_Header *h = array_header(a);                 \
    (a) = array_ensure_capacity(a, n, sizeof(*a));     \
    for (size_t i = 0, j = h->length; i < n; ++i, ++j) \
      (a)[j] = (v)[i];                                 \
    array_header(a)->length += n;                      \
  } while (0)

void *
array_ensure_capacity(void *a, size_t item_count, size_t item_size)
{
  Array_Header *h = array_header(a);
  size_t desired_capacity = h->length + item_count;

  if (h->capacity < desired_capacity) {
    size_t new_capacity = h->capacity * 2;
    while (new_capacity < desired_capacity) {
      new_capacity *= 2;
    }

    size_t new_size = sizeof(Array_Header) + new_capacity * item_size;
    Array_Header *new_h = h->allocator->alloc(new_size, h->allocator->context);

    if (new_h) {
      size_t old_size = sizeof(*h) + h->length * item_size;
      fprintf(stderr, "new_size=%zu old_size=%zu h=%p\n", new_size, old_size,
              (void *)new_h);
      memcpy(new_h, h, old_size);

      if (h->allocator->free) {
        h->allocator->free(old_size, h, h->allocator->context);
      }

      new_h->capacity = new_capacity;
      h = new_h + 1;
    }
    else {
      h = 0;
    }
  }
  else {
    h += 1;
  }

  return h;
}

int
main(int UNUSED(ac), char **UNUSED(av))
{
  int *a = array(int, &my_allocator);
  int *b = array(int, &my_allocator);

  for (short i = 33; i < 37; ++i)
    array_append(b, i);
  array_append_n(a, b, 4);

  for (short i = 0; i < 1024; ++i) {
    array_append(a, i);
    if (i % 3 == 0)
      array_remove(a, array_length(a) - 2);
  }
  size_t cap = array_capacity(a);
  size_t len = array_length(a);
  printf("cap=%zu len=%zu\n", cap, len);
  printf(" 0: %d\n", a[0]);
  printf(" 1: %d\n", a[1]);
  printf("16: %d\n", a[16]);
  assert(a[16] == 19);
  for (size_t i = len / 2; i < len; ++i)
    array_remove(a, i);
  for (size_t i = 0; i < array_length(a); ++i)
    array_remove(a, 0);
  array_free(a);
  array_free(b);
  return 0;
}
