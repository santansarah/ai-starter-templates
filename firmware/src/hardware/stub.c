#include "stub.h"
static struct hardware_reading read_unavailable(void *context)
{
	(void)context;
	return (struct hardware_reading){ .status = HARDWARE_UNAVAILABLE };
}
const struct hardware_provider stub_hardware = { .read = read_unavailable };
