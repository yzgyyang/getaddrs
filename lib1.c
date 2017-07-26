#include <stdint.h>

static int i1;

uintptr_t
lib1_data_addr(void)
{
	return (uintptr_t)&i1;
}
