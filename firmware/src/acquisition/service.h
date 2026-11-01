#ifndef TEMPLATE_ACQUISITION_SERVICE_H
#define TEMPLATE_ACQUISITION_SERVICE_H
#include "state.h"
/* Start once from main before BLE. Provider and context must outlive service. */
int acquisition_start(const struct hardware_provider *provider);
struct acquisition_snapshot acquisition_get_snapshot(void);
#endif
