#ifndef VIRTIO_H
#define VIRTIO_H

#include "types.h"
// virtio mmio control registers, mapped starting at 0x10001000.
// from qemu virtio_mmio.h

#define VIRTIO_MMIO_MAGIC_VALUE 0x000
#define VIRTIO_MMIO_VERSION 0x004
#define VIRTIO_MMIO_DEVICE_ID 0x008
#define VIRTIO_MMIO_VENDOR_ID 0x00C
#define VIRTIO_MMIO_DEVICE_FEATURES 0x010
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL 0x014
#define VIRTIO_MMIO_DRIVER_FEATURES 0x020
#define VIRTIO_MMIO_DRIVER_FEATURES_SEL 0x024
#define VIRTIO_MMIO_QUEUE_SEL 0x030
#define VIRTIO_MMIO_QUEUE_NUM_MAX 0x034
#define VIRTIO_MMIO_QUEUE_NUM 0x038
#define VIRTIO_MMIO_QUEUE_READY 0x044
#define VIRTIO_MMIO_QUEUE_NOTIFY 0x050
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x060
#define VIRTIO_MMIO_INTERRUPT_ACK 0x064
#define VIRTIO_MMIO_STATUS 0x070
#define VIRTIO_MMIO_QUEUE_DESC_LOW 0x080
#define VIRTIO_MMIO_QUEUE_DESC_HIGH 0x084
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW 0x090
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH 0x094
#define VIRTIO_MMIO_QUEUE_USED_LOW 0x0A0
#define VIRTIO_MMIO_QUEUE_USED_HIGH 0x0A4
#define VIRTIO_MMIO_CONFIG_GENERATION 0x0FC
#define VIRTIO_MMIO_CONFIG_BASE 0x100
// virtio mmio interface
#define VIRTIO0 0x10008000
#define VIRTIO0_IRQ 8

// status register bits, from qemu virtio_config.h
#define VIRTIO_CONFIG_S_ACKNOWLEDGE 0x1
#define VIRTIO_CONFIG_S_DRIVER 0x2
#define VIRTIO_CONFIG_S_DRIVER_OK 0x4
#define VIRTIO_CONFIG_S_FEATURES_OK 0x8

#define VIRTIO_GPU_F_VIRGL 0
#define VIRTIO_GPU_F_EDID 1
// device feature bits
#define VIRTIO_F_ANY_LAYOUT 27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX 29

#define R(r) ((volatile uint32 *)(VIRTIO0 + (r)))

#define VIRTIO_QUEUE_SIZE 8

#define VIRTQ_DESC_F_NEXT 0x1
#define VIRTQ_DESC_F_WRITE 0x2
#define VIRTQ_DESC_F_USED 0x4

struct virtq_desc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 next;
};

struct virtq_avail {
  uint16 flags;
  uint16 idx;
  uint16 ring[256];
};

struct virtq_used_elem {
  uint32 id;
  uint32 len;
};

struct virtq_used {
  uint16 flags;
  uint16 idx;
  struct virtq_used_elem ring[256];
};

static int virtq_alloc_desc();

#endif
