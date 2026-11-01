#include "device.h"
struct device_snapshot app_device_evaluate(struct acquisition_snapshot sample,
					 int64_t now_ms, uint32_t stale_ms)
{
	struct device_snapshot result = { .sample = sample, .status = DEVICE_UNAVAILABLE };
	if (sample.status == HARDWARE_FAULT) {
		result.status = DEVICE_FAULT;
	} else if (sample.status == HARDWARE_READY && sample.has_sample) {
		result.status = now_ms < sample.acquired_ms || sample.acquired_ms < 0 ||
			(uint64_t)now_ms - (uint64_t)sample.acquired_ms >= stale_ms
			? DEVICE_STALE : DEVICE_VALID;
	}
	return result;
}
