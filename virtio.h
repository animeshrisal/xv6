#ifndef VIRTIO_H
#define VIRTIO_H

#include "types.h"
// virtio mmio control registers, mapped starting at 0x10001000.
// from qemu virtio_mmio.h

#define VIRTIO_MMIO_MAGIC_VALUE 0x000
#define VIRTIO_MMIO_VERSION 0x004
#define VIRTIO_MMIO_DEVICE_ID 0x008
#define VIRTIO_MMIO_VENDOR_ID 0x00C
#define VIRTIO_MMIO_STATUS 0x010
#define VIRTIO_MMIO_QUEUE_NUM 0x014
#define VIRTIO_MMIO_QUEUE_SEL 0x018
#define VIRTIO_MMIO_QUEUE_READY 0x01C
#define VIRTIO_MMIO_QUEUE_NOTIFY 0x020
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x024
#define VIRTIO_MMIO_INTERRUPT_ACK 0x028
#define VIRTIO_MMIO_DEVICE_FEATURES 0x100
#define VIRTIO_MMIO_DRIVER_FEATURES 0x200

// virtio mmio interface
#define VIRTIO0 0x10008000
#define VIRTIO0_IRQ 1

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
#define BASE(r) ((volatile uint32 *)(r))

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
  uint16 ring[VIRTIO_QUEUE_SIZE];
};

struct virtq_used_elem {
  uint32 id;
  uint32 len;
};

struct virtq_used {
  uint16 flags;
  uint16 idx;
  struct virtq_used_elem ring[VIRTIO_QUEUE_SIZE];
};

struct virtq {
  struct virtq_desc desc[VIRTIO_QUEUE_SIZE];
  struct virtq_avail avail;
  struct virtq_used used;
};

void virtio_gpu_send_command(void *cmd, int len);
void virtq_init(struct virtq *vq);
void virtq_free_desc(struct virtq *vq, int index);
void virtq_add_to_avail(struct virtq *vq, int desc_index);
void virtq_wait_for_used(struct virtq *vq);
int virtq_alloc_desc(struct virtq *vq);

#endif
