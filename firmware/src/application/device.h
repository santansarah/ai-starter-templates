#ifndef TEMPLATE_DEVICE_H
#define TEMPLATE_DEVICE_H
#include "../acquisition/state.h"
enum device_status { DEVICE_UNAVAILABLE = 0, DEVICE_VALID = 1, DEVICE_FAULT = 2, DEVICE_STALE = 3 };
struct device_snapshot {
	struct acquisition_snapshot sample;
	enum device_status status;
};
/* Pure application policy, with an explicit clock and freshness threshold. */
struct device_snapshot app_device_evaluate(struct acquisition_snapshot sample,
					 int64_t now_ms, uint32_t stale_ms);
#endif
