#include "simulated.h"
/* Only the acquisition thread accesses this simulation context. */
static uint32_t step;
static struct hardware_reading read_simulated(void *context)
{
	uint32_t *counter = context;
	uint32_t phase = (*counter)++ % 8;
	/* Exercise signed values, valid zero, failure and subsequent recovery. */
	return (struct hardware_reading){
		.status = phase == 6 ? HARDWARE_FAULT : HARDWARE_READY,
		.value = (int32_t)phase - 3,
	};
}
const struct hardware_provider simulated_hardware = {
	.read = read_simulated, .context = &step,
};
