#include "state.h"
void acquisition_record(struct acquisition_snapshot *state,
			struct hardware_reading reading, int64_t now_ms)
{
	state->status = reading.status;
	if (reading.status == HARDWARE_READY) {
		state->value = reading.value;
		state->sequence++;
		state->acquired_ms = now_ms;
		state->has_sample = true;
	}
}
