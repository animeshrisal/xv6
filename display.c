#include "display.h"
#include "virtq.h"
#define NUM 8

struct virtio_gpu gpu;

void create_2d_resource(int virtio_gpu_fd) {
  struct virtio_gpu_resource_create_2d create_2d_cmd = {
      .ctrl_header.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
      .resource_id = 1,
      .format = VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM,
      .width = FB_WIDTH,
      .height = FB_HEIGHT,
  };
};

void attach_backing(int virtio_gpu_fd, uint64 fb_addr) {
  struct virtio_gpu_mem_entry entry = {
      .addr = fb_addr,
      .length = 10 * 10 * 4,
      .padding = 0,
  };

  struct virtio_gpu_resource_attach_backing attach_backing_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
      .resource_id = 1,
      .nr_entries = 1,
  };
};

void transfer_to_host_2d(int virtio_gpu_fd) {
  struct virtio_gpu_rect rect = {
      .x = 0,
      .y = 0,
      .width = 10,
      .height = 10,
  };

  struct virtio_gpu_transfer_to_host_2d transfer_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
      .r = rect,
      .offset = 0,
      .resource_id = 1,
      .padding = 0,
  };
};

void set_scanout(int virtio_gpu_fd) {
  struct virtio_gpu_rect rect = {.x = 0, .y = 0, .width = 10, .height = 10};

  struct virtio_gpu_resp_display_info display_info_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_SET_SCANOUT,
      .pmodes[0].r = rect,
      .pmodes[0].enabled = 1};
};

void virtq_init(struct virtq *vq) {
  vq->avail = (struct virtq_avail *)kalloc();
  vq->used = (struct virtq_used *)kalloc();

  vq->avail->idx = 0;
  vq->used->idx = 0;
}

void virtio_gpu_send_command(void *cmd, int len) {
  int desc_index = virtq_alloc_desc(&gpu.queue);
  struct virtq_desc *desc = &gpu.queue.desc[desc_index];
  desc->addr = (uint64)cmd;
  desc->len = len;
  desc->flags = 0;

  virtq_add_to_avail(&gpu.queue, desc_index);
  virtq_wait_for_used(&gpu.queue);
  virtq_free_desc(&gpu.queue, desc_index);
}

void virtio_gpu_init() { virtq_init(&gpu.queue); }
