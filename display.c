#include "display.h"
#include "kernel.h"
#include "talloc.h"
#include "virtio.h"

#define NUM 8

static struct gpu {
  struct virtq_desc *desc;
  struct virtq_avail *avail;
  struct virtq_used *used;
} gpu;

// Define feature bits for Virtio GPU
void *memset(void *dst, int c, uint16 n) {
  char *cdst = (char *)dst;
  int i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  return dst;
};

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

void virtio_gpu_init() {
  // Create a 2D resource
  // reset device
  uint32 status = 0;

  if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976) {
    printastring("Could not find virtio\n");
  } else if (*R(VIRTIO_MMIO_DEVICE_ID) != 16) {
    printastring("GPU is not connected!\n");
  }

  // Reset device status
  *R(VIRTIO_MMIO_STATUS) = 0;

  // Set ACKNOWLEDGE status bit
  *R(VIRTIO_MMIO_STATUS) = VIRTIO_CONFIG_S_ACKNOWLEDGE;

  // Set DRIVER status bit
  *R(VIRTIO_MMIO_STATUS) |= VIRTIO_CONFIG_S_DRIVER;

  // Set queue size
  *R(VIRTIO_MMIO_QUEUE_NUM) = 0;

  // Negotiate features
  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);

  // Mask out features that we don't support or need
  features &= (VIRTIO_GPU_F_EDID | VIRTIO_GPU_F_VIRGL);

  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // Notify device that feature negotiation is complete
  *R(VIRTIO_MMIO_STATUS) |= VIRTIO_CONFIG_S_FEATURES_OK;

  // Verify FEATURES_OK is set
  status = *R(VIRTIO_MMIO_STATUS);

  if (!(status & VIRTIO_CONFIG_S_FEATURES_OK)) {
    printastring("ERROR!");
    return;
  }

  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;

  // ensure queue 0 is not in use.
  if (*R(VIRTIO_MMIO_QUEUE_READY)) {
    printastring("virtio disk should not be ready");
  };

  gpu.desc = talloc();
  gpu.avail = talloc();
  gpu.used = talloc();

  if (!gpu.desc || !gpu.avail || !gpu.used) {
    printastring("Could not allocate memory");
    return;
  }

  memset(gpu.desc, 0, 4096);
  memset(gpu.avail, 0, 4096);
  memset(gpu.desc, 0, 4096);

  // set queue size.
  *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;

  *R(VIRTIO_MMIO_QUEUE_DESC_LOW) = (uint64)gpu.desc;
  *R(VIRTIO_MMIO_QUEUE_DESC_HIGH) = (uint64)gpu.desc >> 32;
  *R(VIRTIO_MMIO_QUEUE_AVAIL_LOW) = (uint64)gpu.avail;
  *R(VIRTIO_MMIO_QUEUE_AVAIL_HIGH) = (uint64)gpu.avail >> 32;
  *R(VIRTIO_MMIO_QUEUE_USED_LOW) = (uint64)gpu.used;
  *R(VIRTIO_MMIO_QUEUE_USED_HIGH) = (uint64)gpu.used >> 32;

  // queue is ready.
  *R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

  // Set the DRIVER_OK status bit
  *R(VIRTIO_MMIO_STATUS) |= VIRTIO_CONFIG_S_DRIVER_OK;
  status = *R(VIRTIO_MMIO_STATUS);
  printastring("GPU Driver initialized!");

  struct virtio_gpu_resource_create_2d create_cmd = {
      .ctrl_header.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
      .resource_id = 1,
      .format = VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM,
      .width = 640,
      .height = 480};

  struct virtio_gpu_resource_attach_backing attach_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
      .resource_id = 1,
      .nr_entries = 1,
  };

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
  // Set the scanout to display the framebuffer
  struct virtio_gpu_resp_display_info set_scanout_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_SET_SCANOUT,
      .pmodes[0].r = rect,
      .pmodes[0].enabled = 1,
  };
}
