#include "display.h"
#include "talloc.h"
#include "tprintf.h"
#include "types.h"
#include "virtio.h"

#define NUM 1024

extern uint64 process_id;

static struct gpu {
  struct virtq_desc desc[NUM] __attribute__((aligned(4096)));
  struct virtq_avail avail[NUM] __attribute__((aligned(4096)));
  struct virtq_used used[NUM] __attribute__((aligned(4096)));

  char free[NUM];
  uint16 used_idx;
  Pixel framebuffer[640 * 480];
  uint32 width;
  uint32 height;
} __attribute__((aligned(4096))) gpu;

uint64 get_framebuffer() { return (uint64)gpu.framebuffer; }

void create_2d_resource(int virtio_gpu_fd) {
  struct virtio_gpu_resource_create_2d create_2d_cmd = {
      .ctrl_header.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
      .resource_id = 1,
      .format = VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM,
      .width = 640,
      .height = 480,
  };
};

struct ctrl_header created_2d_response(enum CtrlType type) {
  struct ctrl_header hdr = {.type = type};
  return hdr;
}

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

void fill_rect() {}

void virtio_gpu_init() {
  // Create a 2D resource
  // reset device
  uint32 status = 0;

  if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
      *R(VIRTIO_MMIO_VERSION) != 0x2 || *R(VIRTIO_MMIO_DEVICE_ID) != 16) {

    tprintf("Could not find virtio gpu\n");
    return;
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

  gpu.width = 640;
  gpu.height = 480;

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
  // Verify FEATURES_OK is set
  status = *R(VIRTIO_MMIO_STATUS);

  if (!(status & VIRTIO_CONFIG_S_DRIVER_OK)) {
    tprintf("Could not initialize\n");
    return;
  }

  tprintf("GPU initialized!\n");
  gpu_initialize();
}

static void free_desc(int i) {
  gpu.desc[i].addr = 0;
  gpu.desc[i].len = 0;
  gpu.desc[i].flags = 0;
  gpu.desc[i].next = 0;
  gpu.free[i] = 1;
}

static void free_chain(int i) {
  while (1) {
    int flag = gpu.desc[i].flags;
    int nxt = gpu.desc[i].next;
    free_desc(i);
    if (flag & VIRTQ_DESC_F_NEXT)
      i = nxt;
    else
      break;
  }
}

void virtio_gpu_intr() {

  uint32 status = *R(VIRTIO_MMIO_INTERRUPT_STATUS);
  *R(VIRTIO_MMIO_INTERRUPT_ACK) = status & 0x3;

  while (gpu.used_idx != gpu.used->idx) {
    int id = gpu.used->ring[gpu.used_idx % NUM].id;
    // struct virtq_desc desc = gpu.desc[id];
    free_chain(id);
    gpu.used_idx = gpu.used_idx + 1;
  }
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

static int create_descriptor(void *cmd, int size, uint16 flags) {
  int idx = alloc_desc(); // Allocate a descriptor
  if (idx < 0) {
    tprintf("Failed to allocate descriptor\n");
    return -1;
  }
  gpu.desc[idx].addr = (uint64)cmd; // Set the address of the command
  gpu.desc[idx].len = size;         // Set the size of the command
  gpu.desc[idx].flags =
      flags == VIRTQ_DESC_F_NEXT
          ? VIRTQ_DESC_F_NEXT
          : VIRTQ_DESC_F_WRITE; // Set descriptor flags (e.g., write)
  gpu.desc[idx].next = flags == VIRTQ_DESC_F_NEXT
                           ? (idx + 1) % NUM
                           : 0; // No next descriptor in chain

  return idx;
}

void fill_rects_kernel(int i) {
  int color = 255;
  Pixel white = {.R = color, .G = (color + i) % 256, .B = color, .A = color};

  for (int i = 0; i < 640 * 480; i++) {
    gpu.framebuffer[i] = white;
  }
}

void gpu_initialize() {
  struct virtio_gpu_rect rect = {.x = 0, .y = 0, .width = 640, .height = 480};

  // Command to create a 2D resource
  struct virtio_gpu_resource_create_2d create_cmd = {
      .ctrl_header.type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
      .resource_id = 1,
      .format = VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM,
      .width = 640,
      .height = 480};

  // Command to attach backing to the resource
  struct virtio_gpu_resource_attach_backing attach_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
      .resource_id = 1,
      .nr_entries = 1};

  struct virtio_gpu_cmd_set_scanout scanout_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_SET_SCANOUT,
      .r = {.x = 0, .y = 0, .width = 640, .height = 480},
      .resource_id = 1,
      .scanout_id = 0};

  // Define the rectangle dimensions
  // Command to transfer the 2D resource to the host
  struct virtio_gpu_transfer_to_host_2d transfer_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
      .r = {.x = 0, .y = 0, .width = 640, .height = 480},
      .offset = 0,
      .resource_id = 1};

  // Command to flush the resource
  struct virtio_gpu_resource_flush resource_flush_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_FLUSH,
      .r = {.x = 0, .y = 0, .width = 640, .height = 480},
      .padding = 0,
      .resource_id = 1};

  // Create descriptors for each command and add them to the ring
  // create 2d resource
  int idx =
      create_descriptor(&create_cmd, sizeof(create_cmd), VIRTQ_DESC_F_NEXT);
  struct ctrl_header hdr;

  create_descriptor(&hdr, sizeof(hdr), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;

  // Create descriptors for each command and add them to the ring
  // create 2d resource
  idx = create_descriptor(&attach_cmd, sizeof(attach_cmd), VIRTQ_DESC_F_NEXT);

  struct ctrl_header hdr2;
  struct virtio_gpu_mem_entry mementries = {.addr = (uint64)&gpu.framebuffer,
                                            .length = 640 * 480 * sizeof(Pixel),
                                            .padding = 0};

  create_descriptor(&mementries, sizeof(mementries), VIRTQ_DESC_F_NEXT);
  create_descriptor(&hdr2, sizeof(hdr2), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;

  // Create descriptors for each command and add them to the
  // ring create 2d resource
  idx = create_descriptor(&scanout_cmd, sizeof(scanout_cmd), VIRTQ_DESC_F_NEXT);
  struct ctrl_header hdr3;

  create_descriptor(&hdr3, sizeof(hdr3), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;

  idx =
      create_descriptor(&transfer_cmd, sizeof(transfer_cmd), VIRTQ_DESC_F_NEXT);
  struct ctrl_header hdr4;

  create_descriptor(&hdr4, sizeof(hdr4), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;

  // Create descriptors for each command and add them to the ring
  // create 2d resource
  idx = create_descriptor(&resource_flush_cmd, sizeof(resource_flush_cmd),
                          VIRTQ_DESC_F_NEXT);
  struct ctrl_header hdr5;

  create_descriptor(&hdr5, sizeof(hdr5), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;
  fill_rects_kernel(255);
}

void virtio_gpu_queue_start() {
  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number
}

void transfer() {

  struct virtio_gpu_transfer_to_host_2d transfer_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
      .r = {.x = 0, .y = 0, .width = 640, .height = 480},
      .offset = 0,
      .resource_id = 1};

  // Command to flush the resource
  struct virtio_gpu_resource_flush resource_flush_cmd = {
      .hdr.type = VIRTIO_GPU_CMD_RESOURCE_FLUSH,
      .r = {.x = 0, .y = 0, .width = 640, .height = 480},
      .padding = 0,
      .resource_id = 1};

  int idx;
  idx =
      create_descriptor(&transfer_cmd, sizeof(transfer_cmd), VIRTQ_DESC_F_NEXT);
  struct ctrl_header hdr4;

  create_descriptor(&hdr4, sizeof(hdr4), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;

  // Create descriptors for each command and add them to the ring
  // create 2d resource
  idx = create_descriptor(&resource_flush_cmd, sizeof(resource_flush_cmd),
                          VIRTQ_DESC_F_NEXT);
  struct ctrl_header hdr5;

  create_descriptor(&hdr5, sizeof(hdr5), 0);
  gpu.avail->ring[gpu.avail->idx % NUM] =
      idx; // Add descriptor to the available ring

  gpu.avail->idx += 1;

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number
}
