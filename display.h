#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

#define VIRTIO_MAX_SCANOUTS 16
#define VIRTIO_GPU_FLAG_FENCE (1 << 0)

enum CtrlType {
  /* 2D commands */
  VIRTIO_GPU_CMD_GET_DISPLAY_INFO = 0x0100,
  VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
  VIRTIO_GPU_CMD_RESOURCE_UNREF,
  VIRTIO_GPU_CMD_SET_SCANOUT,
  VIRTIO_GPU_CMD_RESOURCE_FLUSH,
  VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
  VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
  VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING,
  VIRTIO_GPU_CMD_GET_CAPSET_INFO,
  VIRTIO_GPU_CMD_GET_CAPSET,
  VIRTIO_GPU_CMD_GET_EDID,

  /* Cursor commands */
  VIRTIO_GPU_CMD_UPDATE_CURSOR = 0x0300,
  VIRTIO_GPU_CMD_MOVE_CURSOR,

  /* Success responses */
  VIRTIO_GPU_RESP_OK_NODATA = 0x1100,
  VIRTIO_GPU_RESP_OK_DISPLAY_INFO,
  VIRTIO_GPU_RESP_OK_CAPSET_INFO,
  VIRTIO_GPU_RESP_OK_CAPSET,
  VIRTIO_GPU_RESP_OK_EDID,

  /* Error responses */
  VIRTIO_GPU_RESP_ERR_UNSPEC = 0x1200,
  VIRTIO_GPU_RESP_ERR_OUT_OF_MEMORY,
  VIRTIO_GPU_RESP_ERR_INVALID_SCANOUT_ID,
  VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID,
  VIRTIO_GPU_RESP_ERR_INVALID_CONTEXT_ID,
  VIRTIO_GPU_RESP_ERR_INVALID_PARAMETER,
};

enum virtio_gpu_formats {
  VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM = 1,
  VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM = 2,
  VIRTIO_GPU_FORMAT_A8R8G8B8_UNORM = 3,
  VIRTIO_GPU_FORMAT_X8R8G8B8_UNORM = 4,

  VIRTIO_GPU_FORMAT_R8G8B8A8_UNORM = 67,
  VIRTIO_GPU_FORMAT_X8B8G8R8_UNORM = 68,

  VIRTIO_GPU_FORMAT_A8B8G8R8_UNORM = 121,
  VIRTIO_GPU_FORMAT_R8G8B8X8_UNORM = 134,
};

struct ctrl_header {
  enum CtrlType type;
  uint32 flags;
  uint64 fence_id;
  uint32 padding;
};

struct virtio_gpu_resource_create_2d {
  struct ctrl_header ctrl_header;
  uint32 resource_id;
  uint32 format;
  uint32 width;
  uint32 height;
};

struct virtio_gpu_config {
  uint32 events_read;
  uint32 events_clear;
  uint32 num_scanouts;
  uint32 reserved;
};

struct virtio_gpu_rect {
  uint32 x;
  uint32 y;
  uint32 width;
  uint32 height;
};

struct virtio_gpu_resp_display_info {
  struct ctrl_header hdr;
  struct virtio_display_one {
    struct virtio_gpu_rect r;
    uint32 enabled;
    uint32 flags;
  } pmodes[VIRTIO_MAX_SCANOUTS];
};

struct virtio_gpu_transfer_to_host_2d {
  struct ctrl_header hdr;
  struct virtio_gpu_rect r;
  uint64 offset;
  uint32 resource_id;
  uint32 padding;
};

struct virtio_gpu_resource_attach_backing {
  struct ctrl_header hdr;
  uint32 resource_id;
  uint32 nr_entries;
};

struct virtio_gpu_cmd_set_scanout {
  struct ctrl_header hdr;
  struct virtio_gpu_rect r;
  uint32 scanout_id;
  uint32 resource_id;
};

struct virtio_gpu_resource_flush {
  struct ctrl_header hdr;
  struct virtio_gpu_rect r;
  uint32 resource_id;
  uint32 padding;
};

struct virtio_gpu_mem_entry {
  uint64 addr;
  uint32 length;
  uint32 padding;
};

struct virtio_gpu_resource_detach_backing {
  struct ctrl_header hdr;
  uint32 resource_id;
  uint32 padding;
};

struct virtio_gpu_cursor_pos {
  uint32 scanout_id;
  uint32 x;
  uint32 y;
  uint32 padding;
};

struct virtio_gpu_update_cursor {
  struct ctrl_header hdr;
  struct virtio_gpu_cursor_pos pos;
  uint32 resource_id;
  uint32 hot_x;
  uint32 hot_y;
  uint32 padding;
};

void virtq_init();
void virtio_gpu_intr();
void virtio_gpu_draw_rectangle();
static void gpu_initialize();
void transfer();

typedef struct Pixel {
  uint8 R;
  uint8 G;
  uint8 B;
  uint8 A;
} Pixel;

void virtio_gpu_init();
uint64 get_framebuffer();
void virtio_gpu_queue_start();
#endif
