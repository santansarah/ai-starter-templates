#ifndef TEMPLATE_HARDWARE_H
#define TEMPLATE_HARDWARE_H
#include <stdint.h>
enum hardware_status { HARDWARE_UNAVAILABLE = 0, HARDWARE_READY = 1, HARDWARE_FAULT = 2 };
struct hardware_reading {
	enum hardware_status status;
	int32_t value; /* Example signed counts, not a physical unit. */
};
/* Called only by the acquisition thread. Adapters own bus I/O and bounded
 * driver timeouts. Function pointers select drivers, not consumer callbacks. */
struct hardware_provider {
	struct hardware_reading (*read)(void *context);
	void *context;
};
#endif
