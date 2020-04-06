#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define SCREEN_PORT 0x100
#define SCREEN_H 300
#define SCREEN_W 400
int screen_width();
int screen_height();
int draw_sync();

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      // info->width = SCREEN_W;
      // info->height = SCREEN_H;
      info->width = 3;
      info->height = 3;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      
      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
      int s=(ctl->y*screen_width())+ctl->x;
      int a=0;
      for (int i=0; i<ctl->h; i++) {
	      for (int j=0; j<ctl->w; j++) {
	        fb[s+j]=*(ctl->pixels + a++);
	      }
	      s+=screen_width();
      }

      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
