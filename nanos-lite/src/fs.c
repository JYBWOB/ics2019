#include "fs.h"

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
extern size_t fbsync_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
  {"/dev/fbsync", 0, 0, 0, invalid_read, fbsync_write},
  {"/dev/events", 0, 0, 0, events_read, invalid_write},
  {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}


int fs_open(const char *pathname, int flags, int mode){
  for(int i=0; i<NR_FILES; i++) {
    printf("%s, %s\n", pathname, file_table[i].name);
    if(strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("pathname not correct\n");
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  int sz;
  Finfo* f = &file_table[fd];
  if(file_table[fd].read == NULL) {
    len = (f->open_offset+len > f->size)? (f->size - f->open_offset): len;
    sz = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += sz;
  } else {
    sz = file_table[fd].read(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += sz;
  }
  return sz;
}

size_t fs_direct_read(int fd, void *buf, size_t offset, size_t len) {
  size_t open_offset = file_table[fd].open_offset;
  size_t disk_offset = file_table[fd].disk_offset;
  size_t disk_size = file_table[fd].size;
  len = (disk_size>=(len+open_offset))?len:(file_table[fd].size-open_offset);
  int sz = ramdisk_read(buf, disk_offset+offset, len);
  return sz;
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}


size_t fs_lseek(int fd, size_t offset, int whence) {
  if(fd >= NR_FILES || fd < 0) return 0;
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset; 
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  default:
    break;
  }
  if(offset > file_table[fd].size) {
    file_table[fd].open_offset = file_table[fd].size;
  }
  return file_table[fd].open_offset;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  int sz;
  if(file_table[fd].write == NULL) {
    size_t open_offset = file_table[fd].open_offset;
    size_t disk_offset = file_table[fd].disk_offset;
    size_t disk_size = file_table[fd].size;
    len = (disk_size>=(len+file_table[fd].open_offset))?len:(file_table[fd].size-open_offset);
    sz = ramdisk_write(buf, disk_offset + open_offset, len);
  } else {
    sz = file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += sz;
  return sz;
}