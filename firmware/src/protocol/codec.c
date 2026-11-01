#include "codec.h"
static void encode_le(uint64_t value, uint8_t *out, unsigned count)
{
	for (unsigned i = 0; i < count; ++i) { out[i] = (uint8_t)(value >> (8 * i)); }
}
void codec_status(struct device_snapshot snapshot, uint8_t out[STATUS_PACKET_SIZE])
{
	out[0] = 1;
	out[1] = (uint8_t)snapshot.status;
	out[2] = 1; /* Always synthetic in this starter. */
	encode_le(snapshot.sample.sequence, out + 3, 4);
	encode_le((uint64_t)snapshot.sample.acquired_ms, out + 7, 8);
	/* Invalid readings carry no usable value, but retain last success metadata. */
	encode_le(snapshot.status == DEVICE_VALID ? (uint32_t)snapshot.sample.value : 0,
		  out + 15, 4);
}
