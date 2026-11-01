#include <errno.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include "advertising.h"
#include "constants.h"

static const uint8_t service_uuid[] = { TEMPLATE_SERVICE_UUID };
static const struct bt_data advertising[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_UUID128_ALL, service_uuid, sizeof(service_uuid)),
};
static const struct bt_data scan_response[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, TEMPLATE_DEVICE_NAME, sizeof(TEMPLATE_DEVICE_NAME) - 1),
};

static int start_advertising(void)
{
	return bt_le_adv_start(BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONN,
		BT_GAP_ADV_FAST_INT_MIN_2, BT_GAP_ADV_FAST_INT_MAX_2, NULL),
		advertising, ARRAY_SIZE(advertising), scan_response, ARRAY_SIZE(scan_response));
}

static void restart_advertising(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(restart_work, restart_advertising);

static void restart_advertising(struct k_work *work)
{
	(void)work;
	int err = start_advertising();

	if (err && err != -EALREADY) {
		printk("Advertising restart failed: %d\n", err);
		if (err == -ENOMEM || err == -EAGAIN) {
			k_work_reschedule(&restart_work, K_MSEC(100));
		}
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	(void)conn;
	(void)reason;
	k_work_reschedule(&restart_work, K_MSEC(10));
}
BT_CONN_CB_DEFINE(template_callbacks) = { .disconnected = disconnected };

int ble_start(void)
{
	int err = bt_enable(NULL);

	return err ? err : start_advertising();
}
