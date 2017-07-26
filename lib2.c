#include <stdint.h>

static char c2[4 * 1024 * 1024];

uintptr_t
lib2_data_addr(void)
{
	return (uintptr_t)c2;
}
