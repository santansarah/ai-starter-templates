#include <zephyr/sys/printk.h>
#include "acquisition/service.h"
#include "hardware/simulated.h"
#include "ble/advertising.h"
#include "ble/service.h"
int main(void)
{
	int err = acquisition_start(&simulated_hardware);
	if (!err) { err = ble_start(); }
	if (err) {
		printk("Template startup failed: %d\n", err);
		return err;
	}
	ble_publication_start();
	printk("BLE template ready: SYNTHETIC data only\n");
	return 0;
}
