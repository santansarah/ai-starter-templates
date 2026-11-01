# Using the firmware sample as a reference

[Sample README](README.md) · [All templates](../README.md)

Use this sample alongside a separate project as a working reference. Before
building that project, read the [sample README](README.md) and trace the flow
from `src/main.c` through hardware, acquisition, application, protocol, and BLE.
Implement the new application in its own repository. Keep this sample unchanged
unless the developer explicitly requests changes to it.

Follow the demonstrated separation of responsibilities: hardware I/O in adapters,
reading collection and shared snapshots in acquisition, product calculations in
application/domain code, wire bytes in codecs, and subscriptions and delivery in
BLE. Main wires startup. Use these patterns to implement the new project's
requirements, accounting for the [sample's limits](README.md#limits-to-consider).

Treat synthetic data, example UUIDs, timing values, and unencrypted BLE as sample
choices. Define the new application's behavior and settings from its requirements.
Use existing approved designs for extensions; explain architectural needs the
sample does not demonstrate and propose the smallest extension for developer
approval.

Follow the new project's instructions and workflow. Preserve unrelated work,
keep documentation accurate, and obtain explicit direction before committing or
pushing.

## Build and test guidance

Use the [sample's build steps](README.md#build-the-sample) and
[host check](README.md#check-the-sample) to understand how the example is checked.
For the new application, use its selected SDK, toolchain, and board configuration,
and verify its behavior against its own requirements. Passing checks on the
sample does not verify the new application. Report host tests, SDK builds, and
real-device evidence separately.

Follow the developer's workflow and permissions for running commands, installing
tools, flashing devices, and changing device settings. This sample does not grant
authorization to perform those actions in another project.
