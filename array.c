/* https://bytesbeneath.com/p/dynamic-arrays-in-c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"

void *array_init(size_t item_size, size_t capacity)
{
    size_t size = item_size * capacity + sizeof(Array_Header);
    Array_Header *h = (Array_Header *)malloc(size);
    fprintf(stderr, "ini_size=%zu h=%p\n", size, (void *)h);

    if (h) {
        h->length = 0;
        h->capacity = capacity;
	h->item_size = item_size;
        ++h;
    }

    return h;
}

void array_free(void *a)
{
	Array_Header *h = array_header(a);
	free(h);
}

void *array_ensure_capacity(void *a, size_t minCapacity)
{
	Array_Header *h = array_header(a);

	if (h->capacity < minCapacity) {
		size_t new_capacity = h->capacity * 2;
		while (new_capacity < minCapacity)
			new_capacity *= 2;

		size_t new_size = sizeof(*h) + new_capacity * h->item_size;
		h->capacity = new_capacity;
		h = (Array_Header *)realloc(h, new_size);
                fprintf(stderr, "new_size=%zu h=%p\n", new_size, (void *)h);
	}

	return h ? ++h : h;
}

void array_remove(void *a, size_t i)
{
    Array_Header *h = array_header(a);
    if (i == h->length - 1) {
        h->length -= 1;
    } else if (h->length > 1) {
        void *ptr = ((char *)a) + h->item_size * i;
        void *last = ((char *)a) + h->item_size * (--h->length);
        memcpy(ptr, last, h->item_size);
    }
}

