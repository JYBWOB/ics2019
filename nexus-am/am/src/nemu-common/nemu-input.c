#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define I8042_DATA_PORT 0x60
#define KEYDOWN_MASK 0x8000

size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      kbd->keycode = inl(I8042_DATA_PORT);
      kbd->keydown = (kbd->keydown & KEYDOWN_MASK)? 1:0;
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}