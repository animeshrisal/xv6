#include "talloc.h"
#include "tprintf.h"
#include "virtio.h"

#define NUM 128

static struct rng {
  struct virtq_desc *desc;
  struct virtq_avail *avail;
  struct virtq_used *used;

  char free[NUM];
  uint16 used_idx;
  uint8 buffer[8];
} rng;

void virtio_entropy_init() {
  // Create a 2D resource
  // reset device
  uint32 status = 0;

  if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
      *R(VIRTIO_MMIO_VERSION) != 0x2 || *R(VIRTIO_MMIO_DEVICE_ID) != 4) {

    tprintf("Could not find virtio rng\n");
    return;
  }

  // Reset device status
  *R(VIRTIO_MMIO_STATUS) = status;

  // set ACKNOWLEDGE status bit
  status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_STATUS) = status;

  // set DRIVER status bit
  status |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_STATUS) = status;

  tprintf("Set features!\n");

  // tell device that feature negotiation is complete.
  status |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  // Verify FEATURES_OK is set
  status = *R(VIRTIO_MMIO_STATUS);

  if (!(status & VIRTIO_CONFIG_S_FEATURES_OK)) {
    tprintf("Could not negotiate features\n");
    return;
  }

  tprintf("Negotiated feature!\n");
  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;

  // ensure queue 0 is not in use.
  if (*R(VIRTIO_MMIO_QUEUE_READY)) {
    tprintf("Queue should not be ready\n");
  };

  // check maximum queue size.
  uint32 max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
  if (max == 0)
    tprintf("virtio disk has no queue 0");
  if (max < NUM)
    tprintf("virtio disk max queue too short");
  tprintf("Allocating memory!\n");
  rng.desc = talloc();
  rng.avail = talloc();
  rng.used = talloc();

  if (!rng.desc || !rng.avail || !rng.used) {
    tprintf("Could not allocate memory\n");
    return;
  }

  tmemset(rng.desc, 0, 4096);
  tmemset(rng.avail, 0, 4096);
  tmemset(rng.desc, 0, 4096);
  //  set queue size.
  *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;

  *R(VIRTIO_MMIO_QUEUE_DESC_LOW) = (uint64)rng.desc;
  *R(VIRTIO_MMIO_QUEUE_DESC_HIGH) = (uint64)rng.desc >> 32;
  *R(VIRTIO_MMIO_QUEUE_AVAIL_LOW) = (uint64)rng.avail;
  *R(VIRTIO_MMIO_QUEUE_AVAIL_HIGH) = (uint64)rng.avail >> 32;
  *R(VIRTIO_MMIO_QUEUE_USED_LOW) = (uint64)rng.used;
  *R(VIRTIO_MMIO_QUEUE_USED_HIGH) = (uint64)rng.used >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

  for (int i = 0; i < NUM; i++)
    rng.free[i] = 1;

  // tell device we're completely ready.
  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;
  // Verify FEATURES_OK is set
  status = *R(VIRTIO_MMIO_STATUS);

  if (!(status & VIRTIO_CONFIG_S_DRIVER_OK)) {
    tprintf("Could not initialize\n");
    return;
  }

  tprintf("rng initialized!\n");
}

void get_rng() {}
