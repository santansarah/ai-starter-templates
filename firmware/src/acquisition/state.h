#ifndef TEMPLATE_ACQUISITION_STATE_H
#define TEMPLATE_ACQUISITION_STATE_H
#include <stdbool.h>
#include "../hardware/hardware.h"
struct acquisition_snapshot {
	int32_t value;
	uint32_t sequence;
	int64_t acquired_ms;
	enum hardware_status status;
	bool has_sample;
};
/* Caller owns synchronization. Successful sequence wraps modulo 2^32. */
void acquisition_record(struct acquisition_snapshot *state,
			struct hardware_reading reading, int64_t now_ms);
#endif
