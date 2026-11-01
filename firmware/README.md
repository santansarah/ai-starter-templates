# Nordic BLE firmware sample project

[All templates](../README.md) · [AI guidance](AGENTS.md)

A C sample project for Nordic nRF52840 devices using the nRF Connect SDK and
Zephyr. The sample targets the nRF52840 DK board. It demonstrates how to organize
startup, hardware communication, sensor readings, decisions about those readings,
and sending data over Bluetooth Low Energy (BLE).

The architectural ideas can inform projects on other platforms, such as ESP32,
but their hardware access, Bluetooth APIs, configuration, and build steps need
platform-specific implementations. This sample has not been validated for those
platforms.

Use this sample alongside a new firmware project as a working reference for your
AI coding assistant. Have the assistant read this README, [AGENTS.md](AGENTS.md), and the
source code, then follow the demonstrated patterns while building your application
in its own repository. Your project's requirements define its hardware, behavior,
and protocol; the sample provides examples of how to implement them.

**Full disclosure:** I'm not a C expert. I only dabble. But I am a fan of good architecture, and I
wrote this template with AI. Improvements and suggestions are completely welcome;
I'd love to hear your feedback.

See how it's used in my [Building with AI newsletter](https://www.linkedin.com/newsletters/building-with-ai-7506732844778582018/)
and look for the Pet Scale project.

## Working blueprint

The flow is: read from a sensor → keep the latest result → decide whether it is
usable → format a message → make it available over Bluetooth.

**Acquisition** means repeatedly collecting readings from a sensor. The hardware
adapter handles communication with the sensor; the acquisition service runs the
reading loop and stores the latest result. This sample uses a simulated sensor
that generates example readings, so no physical sensor is needed.

A **snapshot** is a consistent copy of the latest result: the reading, its status,
and information about when it was collected. Other parts of the application can
use that copy without waiting for another sensor read.

| Module | What it does in the sample |
| --- | --- |
| `src/main.c` | Choose the source of readings and start the reading loop and Bluetooth services. |
| `src/hardware/` | Define how to read from a sensor. Provide a simulated sensor and an example source that reports itself unavailable. |
| `src/acquisition/service.c` | Collect readings in a separate task and safely share the latest result with the rest of the application. |
| `src/acquisition/state.c` | Track successful readings and failed attempts, keeping the last successful reading's number and time. |
| `src/application/device.c` | Decide whether the latest result is usable, too old, unavailable, or affected by a sensor error. |
| `src/protocol/codec.c` | Turn the result into the agreed sequence of bytes for a Bluetooth message. |
| `src/ble/service.c` | Let a connected app request the current result or subscribe to receive it at regular intervals. |
| `src/ble/advertising.c` | Make the device discoverable over Bluetooth and resume advertising after an app disconnects. |

The simulated adapter cycles signed counts from -3 through 4, with a fault at
step 6, then repeats. These are synthetic counts, not physical measurements.
The unavailable stub remains an alternative provider. No physical sensor is
required for the simulated readings. The adapter function pointer selects the
driver; consumers do not register callbacks with acquisition.

Acquisition runs in its own thread even with no client. The provider and context
have static lifetime and are accessed only by that thread. Hardware reads occur
outside the snapshot mutex; a short locked section replaces the entire sample.
Readers copy the entire snapshot under the same mutex. No hardware I/O occurs in
GATT callbacks. Adapters must use bounded bus timeouts so failures can be reported.

A successful read updates signed value, success sequence and monotonic acquisition
time. A failure updates status while retaining the last success metadata. Sequence
wraps modulo 2^32 and resets on reboot. The application marks a sample stale at
age >= the configured threshold, or for an inconsistent clock. A fault takes
precedence over staleness. Valid zero is distinct from unavailable/fault/stale.
Application policy, acquisition state transitions and codec are Zephyr-independent.

BLE publication runs independently on Zephyr's system workqueue. Each interval
it enumerates current connections, checks their CCCD subscription, copies current
state and encodes it. It does not queue old samples or retain connection pointers.
Repeated publication can carry the same success sequence: it is not a new sample.
After reconnection a client must subscribe again; packets are obtained from current
state. Acquisition timestamps describe device uptime, not wall time or durable
identity. Products needing cross-reboot identity must design that extension.
Notification failures are logged; the next interval retries current state.
`bt_gatt_notify` copies the packet before returning, so stack bytes need not outlive
that call. Slow hardware work must never be added to this BLE workqueue path.

`Kconfig` supplies acquisition/publication delays (500 ms each) and a stale
threshold (1500 ms). Acquisition delay begins after each read completes; it is not
a guaranteed sampling frequency. These are demonstration settings, not product
requirements. Define hardware wiring in your own project's devicetree overlays
when selecting a device.

## Example BLE contract

Advertising name: `BLE-Template`. Example UUIDs are in `src/ble/constants.h`.
One connected client can read or subscribe to the status characteristic. Each
packet is 19 bytes, fitting the default ATT MTU notification payload:

| Offset | Size | Meaning |
| --- | --- | --- |
| 0 | 1 | Format version 1 |
| 1 | 1 | 0 unavailable, 1 valid, 2 fault, 3 stale |
| 2 | 1 | Source marker 1: synthetic example data |
| 3 | 4 | Successful-sample sequence, unsigned little-endian |
| 7 | 8 | Last successful acquisition uptime in ms, unsigned little-endian |
| 15 | 4 | Signed two's-complement example counts, little-endian; zero when invalid |

Before the first success, sequence and timestamp are zero. Status determines
whether value bytes are usable. The synthetic marker is intentionally always set
in this demonstration codec; a real product must define its own source semantics,
units and protocol. Generate new UUIDs for new products; preserve identifiers of
existing products. BLE access is unencrypted. This blueprint does not demonstrate
commands, persistence, bonding, firmware update, power management or product
business rules. Design those features in your own project when its requirements
are established.

## Limits to consider

- **Polling sensors:** The loop waits after each read. Interrupt-driven devices
  and strict sampling rates need a different acquisition design.
- **Project size:** The separate layers demonstrate responsibilities. A tiny
  application may benefit from fewer modules.
- **Shared workqueue:** Slow work on Zephyr's system workqueue can delay BLE
  notifications. Assess whether your project needs a dedicated workqueue.
- **BLE access:** The sample supports one client and unencrypted access. Decide
  client capacity and security needs, including pairing, bonding, and encryption,
  early in your project's design.
- **Verification:** Host tests cover pure logic. Thread timing, synchronization,
  and real BLE behavior remain unverified on hardware.
- **Device uptime:** Timestamps and sample numbers reset on reboot. They do not
  provide wall-clock time or identity across restarts.
- **AI reuse:** Review what the assistant carries over. Synthetic data markers,
  demo timing values, and unencrypted access are examples, not product defaults.

## Build the sample

To build this sample, use an installed nRF Connect SDK workspace and its matching
toolchain. The sample was previously built with NCS 3.4.1; other SDK versions and
boards need their own validation. Run from the `firmware/` directory in an
SDK-enabled shell:

```sh
west build --sysbuild -p always -b nrf52840dk/nrf52840 -d build .
```

The sample uses simulated readings and provides no sensor wiring or physical
sensor driver. Your new project needs its own build configuration, board overlays,
and hardware adapter for the selected device.

## Check the sample

Run from the `firmware/` directory with a host C compiler:

```sh
cc -std=c11 -Wall -Wextra -Werror -Isrc tests/test_core.c \
  src/acquisition/state.c src/application/device.c src/hardware/stub.c \
  src/hardware/simulated.c src/protocol/codec.c -o /tmp/ble-template-core-test
/tmp/ble-template-core-test
```

The test checks exact negative-value encoding, signed boundaries, valid zero,
failure metadata, recovery, stale threshold, clock inconsistency, sequence wrap,
and the simulated adapter. It does not simulate Zephyr scheduling, mutexes or BLE.

To verify the sample on a device, flash the intended board, confirm synthetic startup,
discover the UUIDs and read status. Subscribe and observe successive sequences and
fault/recovery packets; unsubscribe and verify notifications stop. Disconnect,
allow acquisition to continue, then reconnect and subscribe: confirm current
sequence/time rather than replay. Record client and board details and source/image
identity. Separately exercise concurrent snapshot access and delayed acquisition
on a target/test harness to verify synchronization and stale delivery at runtime.

## Sample verification history

On 2026-10-14 this sample passed the core host check with warnings
treated as errors and the NCS 3.4.1 / Zephyr 4.4.2 sysbuild for
`nrf52840dk/nrf52840`. The image uses 112,708 bytes of flash and 24,552 bytes
of RAM. No device was flashed. Real BLE, scheduling and concurrency behavior
remain unverified until exercised at runtime. These results describe the sample;
they do not verify an application built using it as a reference.

## Using the sample in a new project

Point your AI coding assistant to this sample when starting your new repository.
Use its module boundaries and executable data flow as the architectural reference,
then implement your project's requirements in that repository.

For example, follow the hardware adapter pattern to implement bounded driver
reads, select the adapter in your project's startup code, and register its source
in your build configuration. Follow the acquisition/snapshot pattern for sharing
readings. Put product rules in application/domain code and implement codecs and
acceptance tests for your agreed protocol. Keep hardware data, product meaning,
and transport separate.

Keep the sample available as a reference as development continues. Review and test
your own application's behavior, including hardware failures and recovery, against
its requirements.
