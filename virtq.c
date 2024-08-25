#include "types.h"

#define VIRTIO_QUEUE_SIZE 128

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
  uint16 used_event;
};

struct virtq_used_elem {
  uint32 id;
  uint32 len;
};

struct virtq_used {
  uint16 flags;
  uint16 idx;
  struct virtq_used_elem ring[VIRTIO_QUEUE_SIZE];
  uint16 avail_event;
};

struct virtq {
  struct virtq_desc desc[VIRTIO_QUEUE_SIZE];
  struct virtq_avail *avail;
  struct virtq_used *used;
};
