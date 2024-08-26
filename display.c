#include "display.h"
#include "virtio.h"
#include "virtq.h"

#define NUM 8

static struct gpu {
  struct virtq_desc *desc;
  struct virtq_avail *avail;
  struct virtq_used *used;

  char free[NUM];
  uint16 used_idx;
} gpu;

void *memset(void *dst, int c, uint16 n) {
  char *cdst = (char *)dst;
  int i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  return dst;
}

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

void virtq_initiazation() {

  *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;
  *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;
}

void virtio_gpu_send_command(void *cmd, int len) {}

void virtio_gpu_init() {
  virtq_init();
  // Create a 2D resource
  struct virtio_gpu_resource_create_2d create_cmd = {
      .ctrl_header.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
      .resource_id = 1,
      .format = VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM,
      .width = 640,
      .height = 480};
  virtio_gpu_send_command(&create_cmd, sizeof(create_cmd));

  struct virtio_gpu_resource_attach_backing attach_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
      .resource_id = 1,
      .nr_entries = 1,
  };
  virtio_gpu_send_command(&attach_cmd, sizeof(attach_cmd));

  // Transfer the framebuffer to the GPU
  struct virtio_gpu_rect rect = {
      .x = 0,
      .y = 0,
      .width = 640,
      .height = 480,
  };
  struct virtio_gpu_transfer_to_host_2d transfer_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
      .r = rect,
      .offset = 0,
      .resource_id = 1,
  };
  virtio_gpu_send_command(&transfer_cmd, sizeof(transfer_cmd));

  // Set the scanout to display the framebuffer
  struct virtio_gpu_resp_display_info set_scanout_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_SET_SCANOUT,
      .pmodes[0].r = rect,
      .pmodes[0].enabled = 1,
  };
  virtio_gpu_send_command(&set_scanout_cmd, sizeof(set_scanout_cmd));
};

void virtio_disk_intr() {
  *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;

  while (gpu.used_idx != gpu.used->idx) {
    int id = gpu.used->ring[gpu.used_idx % NUM].id;

    gpu.used_idx += 1;
  }
}
