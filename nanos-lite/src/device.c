#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  // _yield();
  for (size_t i = 0; i < len; ++i)
    _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  // _yield();
  int i=0;
  while(i<len) {
    int key = read_key();
    int down = 0;
    if (key & 0x8000) {
      key ^= 0x8000;
      down = 1;
    }
    if (key != _KEY_NONE) {
      if(down) {
        sprintf(buf, "kd %s\n", keyname[key]);
      } else {
        sprintf(buf, "ku %s\n", keyname[key]);
      }
      i += strlen(keyname[key])+4;
      break;
    } 
    else {
      int t = uptime();
      sprintf(buf, "t %d\n", t);
      i += strlen(buf);
      break;
    }
  }
  return i;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if(offset > strlen(dispinfo)) return 0;
  len = (offset+len>strlen(dispinfo))?(strlen(dispinfo)-offset):len;
  memcpy(buf, dispinfo+offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  // _yield();
  int l = len;
  int w = screen_width();
  int h = screen_height();
  offset >>= 2;
  len >>= 2;
  if(offset >= w*h) return 0;
  int x = offset % w;
  int y = offset / w;
  len = ((offset+len)>=w*h)?(w*h-offset-1):len;
  int _end = offset + len;
  while(offset < _end) {
    int width_w = (w-x)>len?len:(w-x);
    draw_rect((uint32_t*)buf, x, y,  width_w, 1);
    y += 1;
    offset = y * w;
    x = offset % w;
    len -= width_w;
  }
  return l;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}
