#ifndef TEMPLATE_CODEC_H
#define TEMPLATE_CODEC_H
#include <stdint.h>
#include "../application/device.h"
#define STATUS_PACKET_SIZE 19
void codec_status(struct device_snapshot snapshot, uint8_t out[STATUS_PACKET_SIZE]);
#endif
