#include "virtio.h"

struct virtq queue;

void virtq_free_desc(struct virtq *vq, int index) {
  vq->desc[index].flags = 0;
};

void virtq_add_to_avail(struct virtq *vq, int desc_index) {
  vq->avail.ring[vq->avail.idx % VIRTIO_QUEUE_SIZE] = desc_index;
  vq->avail.idx++;
};

void virtq_wait_for_used(struct virtq *vq) {
  while (vq->used.idx == vq->avail.idx) {
  }
};

int virtq_alloc_desc(struct virtq *vq) {
  for (int i = 0; i < VIRTIO_QUEUE_SIZE; i++) {
    if (!(vq->desc[i].flags)) {
      vq->desc[i].flags = 1;
      return i;
    }
  }
  return -1;
};

void virtio_gpu_send_command(void *cmd, int len) {
  int desc_index = virtq_alloc_desc(&queue);
  struct virtq_desc *desc = &queue.desc[desc_index];

  desc->addr = (uint64)cmd;
  desc->len = len;
  desc->flags = 0;

  virtq_add_to_avail(&queue, desc_index);

  virtq_wait_for_used(&queue);
  virtq_free_desc(&queue, desc_index);
}
