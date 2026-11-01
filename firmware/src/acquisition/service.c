#include <errno.h>
#include <zephyr/kernel.h>
#include "service.h"

K_MUTEX_DEFINE(snapshot_lock);
static struct acquisition_snapshot latest;
static const struct hardware_provider *source;
static bool started;
static struct k_thread thread;
K_THREAD_STACK_DEFINE(acquisition_stack, 1024);

static void acquire(void *a, void *b, void *c)
{
	(void)a; (void)b; (void)c;
	for (;;) {
		/* No lock held during potentially blocking hardware access. */
		struct hardware_reading reading = source->read(source->context);
		int64_t now = k_uptime_get();
		k_mutex_lock(&snapshot_lock, K_FOREVER);
		acquisition_record(&latest, reading, now);
		k_mutex_unlock(&snapshot_lock);
		k_sleep(K_MSEC(CONFIG_TEMPLATE_ACQUISITION_INTERVAL_MS));
	}
}

int acquisition_start(const struct hardware_provider *provider)
{
	if (started) { return -EALREADY; }
	if (!provider || !provider->read) { return -EINVAL; }
	source = provider;
	started = true;
	k_thread_create(&thread, acquisition_stack,
		K_THREAD_STACK_SIZEOF(acquisition_stack), acquire, NULL, NULL, NULL,
		5, 0, K_NO_WAIT);
	return 0;
}

struct acquisition_snapshot acquisition_get_snapshot(void)
{
	k_mutex_lock(&snapshot_lock, K_FOREVER);
	struct acquisition_snapshot copy = latest;
	k_mutex_unlock(&snapshot_lock);
	return copy;
}
