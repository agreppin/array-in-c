#include "array.h"
#include "unused.h"
#include <assert.h>
#include <stdio.h>

int
main(int UNUSED(ac), char **UNUSED(av))
{
  int *a = array(int, 16);
  int *b = array(int, 16);

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
