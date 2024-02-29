/* https://bytesbeneath.com/p/dynamic-arrays-in-c */
#pragma once
#include <stddef.h>

typedef struct {
	size_t capacity;
	size_t item_size;
	size_t length;
	size_t padding;
} Array_Header;

#define array(T, capacity) (T *)array_init(sizeof(T), capacity)

void *array_init(size_t item_size, size_t capacity);
void array_free(void *a);
void *array_ensure_capacity(void *a, size_t item_count);
void array_remove(void *a, size_t index);

static inline Array_Header *array_header(void *a) {
	return ((Array_Header *)(a) - 1);
}

static inline size_t array_capacity(void *a) {
	return array_header(a)->capacity;
}

static inline size_t array_length(void *a) {
	return array_header(a)->length;
}

#ifdef __cplusplus
template <typename T, typename U>
static inline void array_append(T *&a, U v)
{
	Array_Header *h = array_header(a);
	a = (T *)array_ensure_capacity(a, h->length + 1);
	a[array_header(a)->length++] = (v);
}
#else
#define array_append(a, v) do {				\
	Array_Header *h = array_header(a);		\
	(a) = array_ensure_capacity(a, h->length + 1);	\
	(a)[array_header(a)->length++] = (v);		\
} while (0)
#endif
