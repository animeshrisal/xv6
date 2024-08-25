#ifndef VIRTQ_H
#define VIRTQ_H

#include "types.h"

#define VIRTIO_QUEUE_SIZE 8

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

void virtq_free_desc(struct virtq *vq, int index);
void virtq_add_to_avail(struct virtq *vq, int desc_index);
void virtq_wait_for_used(struct virtq *vq);
int virtq_alloc_desc(struct virtq *vq);

#endif
