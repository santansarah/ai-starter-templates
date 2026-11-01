#ifndef TEMPLATE_BLE_CONSTANTS_H
#define TEMPLATE_BLE_CONSTANTS_H
#include <zephyr/bluetooth/uuid.h>

/* Example identifiers only. Generate new UUIDs when creating a product. */
#define TEMPLATE_DEVICE_NAME "BLE-Template"
#define TEMPLATE_SERVICE_UUID BT_UUID_128_ENCODE(0xc171b1c0, 0xe748, 0x4e9c, 0xa132, 0xe02cc2ab5901)
#define TEMPLATE_STATUS_UUID BT_UUID_128_ENCODE(0x4c891e20, 0x4d9e, 0x449a, 0xb15f, 0xc22d0c9ca913)
#endif
