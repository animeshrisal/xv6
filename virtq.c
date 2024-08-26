#include "virtq.h"
#include "types.h"
#include "virtio.h"

void virtq_init(struct virtq *vq) {

  uint32 status = 0;

  *R(VIRTIO_MMIO_STATUS) = status;

  // set ACKNOWLEDGE status bit
  *R(VIRTIO_MMIO_STATUS) = status;

  vq->avail->idx = 0;
  vq->used->idx = 0;
};

// allocate a free descriptor
int virtq_alloc_desc(struct virtq *vq) {
  for (int i = 0; i < VIRTIO_QUEUE_SIZE; i++) {
    if (!(vq->desc[i].flags)) {
      vq->desc[i].flags = 1;
      return i;
    }
  }

  return -1;
};

void virtq_free_desc(struct virtq *vq, int index) {
  vq->desc[index].flags = 0;
};

void virtq_add_to_avail(struct virtq *vq, int desc_index) {
  vq->avail->ring[vq->avail->idx % VIRTIO_QUEUE_SIZE] = desc_index;
  vq->avail->idx++;
};

void virtq_wait_for_used(struct virtq *vq) {
  while (vq->used->idx == vq->avail->idx) {
  }
};
