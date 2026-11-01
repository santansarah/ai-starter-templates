#include <assert.h>
#include <limits.h>
#include <string.h>
#include "acquisition/state.h"
#include "application/device.h"
#include "hardware/simulated.h"
#include "hardware/stub.h"
#include "protocol/codec.h"

int main(void)
{
	struct acquisition_snapshot state = {0};
	assert(app_device_evaluate(state, 0, 1500).status == DEVICE_UNAVAILABLE);
	acquisition_record(&state, (struct hardware_reading){HARDWARE_READY, -2}, 100);
	assert(state.sequence == 1 && state.acquired_ms == 100);
	assert(app_device_evaluate(state, 1599, 1500).status == DEVICE_VALID);
	assert(app_device_evaluate(state, 1600, 1500).status == DEVICE_STALE);
	assert(app_device_evaluate(state, 99, 1500).status == DEVICE_STALE);
	uint8_t bytes[STATUS_PACKET_SIZE];
	codec_status(app_device_evaluate(state, 100, 1500), bytes);
	const uint8_t expected[] = {1, 1, 1, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0,
		0xfe, 0xff, 0xff, 0xff};
	assert(memcmp(bytes, expected, sizeof(expected)) == 0);
	/* A failed attempt retains the last success's identity and timestamp. */
	acquisition_record(&state, (struct hardware_reading){HARDWARE_FAULT, 999}, 200);
	assert(state.sequence == 1 && state.acquired_ms == 100 && state.value == -2);
	codec_status(app_device_evaluate(state, 200, 1500), bytes);
	assert(bytes[1] == DEVICE_FAULT && bytes[15] == 0 && bytes[18] == 0);
	acquisition_record(&state, stub_hardware.read(stub_hardware.context), 300);
	assert(app_device_evaluate(state, 300, 1500).status == DEVICE_UNAVAILABLE);
	/* Zero is a real, successful value after recovery. */
	acquisition_record(&state, (struct hardware_reading){HARDWARE_READY, 0}, 400);
	assert(state.sequence == 2 && state.acquired_ms == 400);
	assert(app_device_evaluate(state, 400, 1500).status == DEVICE_VALID);
	const int32_t boundaries[] = {INT32_MIN, INT32_MAX};
	for (unsigned i = 0; i < 2; ++i) {
		acquisition_record(&state, (struct hardware_reading){HARDWARE_READY, boundaries[i]}, 500);
		codec_status(app_device_evaluate(state, 500, 1500), bytes);
		assert(bytes[15] == (i == 0 ? 0 : 255));
		assert(bytes[18] == (i == 0 ? 128 : 127));
	}
	state.sequence = UINT32_MAX;
	acquisition_record(&state, (struct hardware_reading){HARDWARE_READY, 1}, 600);
	assert(state.sequence == 0);
	/* Exercise the actual example adapter across failure and recovery. */
	for (unsigned i = 0; i < 8; ++i) {
		struct hardware_reading reading = simulated_hardware.read(simulated_hardware.context);
		assert(reading.status == (i == 6 ? HARDWARE_FAULT : HARDWARE_READY));
		assert(reading.value == (int32_t)i - 3);
	}
	return 0;
}
