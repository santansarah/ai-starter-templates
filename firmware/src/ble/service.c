#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "constants.h"
#include "service.h"
#include "../acquisition/service.h"
#include "../application/device.h"
#include "../protocol/codec.h"

static void encode_current(uint8_t bytes[STATUS_PACKET_SIZE])
{
	struct acquisition_snapshot sample = acquisition_get_snapshot();
	codec_status(app_device_evaluate(sample, k_uptime_get(),
		CONFIG_TEMPLATE_STALE_MS), bytes);
}
static ssize_t read_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			   void *buf, uint16_t len, uint16_t offset)
{
	uint8_t bytes[STATUS_PACKET_SIZE];
	encode_current(bytes);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, bytes, sizeof(bytes));
}
BT_GATT_SERVICE_DEFINE(template_service,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(TEMPLATE_SERVICE_UUID)),
	BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(TEMPLATE_STATUS_UUID),
		BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ,
		read_status, NULL, NULL),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

static void notify_peer(struct bt_conn *conn, void *data)
{
	(void)data;
	const struct bt_gatt_attr *attr = &template_service.attrs[2];
	if (!bt_gatt_is_subscribed(conn, attr, BT_GATT_CCC_NOTIFY)) { return; }
	/* A fresh snapshot per delivery; no retained connection or replay queue.
	 * bt_gatt_notify copies these stack bytes before returning. */
	uint8_t bytes[STATUS_PACKET_SIZE];
	encode_current(bytes);
	int err = bt_gatt_notify(conn, attr, bytes, sizeof(bytes));
	if (err) { printk("Template notification failed: %d\n", err); }
}
static void publish(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(publication_work, publish);
static void publish(struct k_work *work)
{
	(void)work;
	bt_conn_foreach(BT_CONN_TYPE_LE, notify_peer, NULL);
	k_work_reschedule(&publication_work, K_MSEC(CONFIG_TEMPLATE_PUBLICATION_INTERVAL_MS));
}
void ble_publication_start(void)
{
	k_work_reschedule(&publication_work, K_NO_WAIT);
}
