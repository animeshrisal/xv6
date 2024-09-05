#include "display.h"
#include "kernel.h"
#include "talloc.h"
#include "tprintf.h"
#include "virtio.h"

#define NUM 8

static struct gpu {
  struct virtq_desc *desc;
  struct virtq_avail *avail;
  struct virtq_used *used;

  char free[NUM];
  uint16 used_idx;
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
    tprintf("Could not find the magic value!\n");

  } else if (*R(VIRTIO_MMIO_DEVICE_ID) != 16) {
    tprintf("Could not find driver device id!\n");
  }

  // Reset device status
  *R(VIRTIO_MMIO_STATUS) = status;

  // set ACKNOWLEDGE status bit
  status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_STATUS) = status;

  // set DRIVER status bit
  status |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_STATUS) = status;

  uint64 features = *R(VIRTIO_MMIO_DEVICE_FEATURES);

  // Mask out features that we don't support or need
  features &= (VIRTIO_GPU_F_EDID | VIRTIO_GPU_F_VIRGL);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

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

  gpu.desc = talloc();
  gpu.avail = talloc();
  gpu.used = talloc();

  tprintf("Allocating memory!\n");
  if (!gpu.desc || !gpu.avail || !gpu.used) {
    tprintf("Could not allocate memory\n");
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

  for (int i = 0; i < NUM; i++)
    gpu.free[i] = 1;

  // tell device we're completely ready.
  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;
  tprintf("GPU initialized\n");

  uint32 status2 = *R(VIRTIO_MMIO_STATUS);
  gpu_initialize();
}

void virtio_gpu_intr() {
  *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;
  tprintf("Virtio gpu interrupted");
}

static int alloc_desc() {
  for (int i = 0; i < NUM; i++) {
    if (gpu.free[i]) {
      gpu.free[i] = 0;
      return i;
    }
  }
  return -1;
}

static void free_desc(int i) {
  gpu.desc[i].addr = 0;
  gpu.desc[i].len = 0;
  gpu.desc[i].flags = 0;
  gpu.desc[i].next = 0;
  gpu.free[i] = 1;
}

static void create_descriptor(void *cmd, int size) {
  int idx = alloc_desc();           // Allocate a descriptor
  gpu.desc[idx].addr = (uint64)cmd; // Set the address of the command
  gpu.desc[idx].len = size;         // Set the size of the command
  gpu.desc[idx].flags =
      VIRTQ_DESC_F_WRITE; // Set descriptor flags (e.g., write)
  //
  gpu.desc[idx].next = 0; // No next descriptor in chain

  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx;             // Add descriptor to the available ring
  gpu.avail->idx += 1; // Update the available index

  free_desc(idx); // Free the descriptor after command has been used
}

void gpu_initialize() {
  // Command to create a 2D resource
  struct virtio_gpu_resource_create_2d create_cmd = {
      .ctrl_header.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
      .resource_id = 1,
      .format = VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM,
      .width = 1920,
      .height = 1080};

  // Command to attach backing to the resource
  struct virtio_gpu_resource_attach_backing attach_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
      .resource_id = 1,
      .nr_entries = 1};

  // Define the rectangle dimensions
  struct virtio_gpu_rect rect = {.x = 0, .y = 0, .width = 640, .height = 480};

  // Command to transfer the 2D resource to the host
  struct virtio_gpu_transfer_to_host_2d transfer_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
      .r = rect,
      .offset = 0,
      .resource_id = 1};

  // Set the scanout to display the framebuffer
  struct virtio_gpu_resp_display_info set_scanout_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_SET_SCANOUT,
      .pmodes[0].r = rect,
      .pmodes[0].enabled = 1,
  }; // Create descriptors for each command and add them to the ring
  create_descriptor(&create_cmd, sizeof(create_cmd));
  create_descriptor(&attach_cmd, sizeof(attach_cmd));
  create_descriptor(&transfer_cmd, sizeof(transfer_cmd));
  create_descriptor(&set_scanout_cmd, sizeof(set_scanout_cmd));

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number
}
