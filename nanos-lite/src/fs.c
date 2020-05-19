#include "fs.h"

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

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
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}


int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      //file_table[i].open_offset = 1;
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  assert(0 <= fd && fd < NR_FILES);
  size_t res;
	if(file_table[fd].size && file_table[fd].open_offset + len > file_table[fd].size) {
		len = file_table[fd].size - file_table[fd].open_offset;
	}
  if(file_table[fd].read == NULL) {
  	res = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	}
	else {
    res = file_table[fd].read(buf, file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += res;
  return res;
}

int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}


size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(0 <= fd && fd < NR_FILES);
  switch(whence) {
		case SEEK_SET: file_table[fd].open_offset = offset; break;
		case SEEK_CUR: file_table[fd].open_offset += offset; break;
		case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
		default: panic("fs_lseek error!");
  }
  return file_table[fd].open_offset;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  assert(0 <= fd && fd < NR_FILES);
  // if(fd==1||fd==2) {
	// 	for(int i=0;i<len;i++)
	// 	  _putc(*(char*)(buf+i));
  //   return len;
  // } 
  size_t res;
  if(file_table[fd].write==NULL) {
	  res=len;
	  if(file_table[fd].open_offset + len > file_table[fd].size)
		  res = file_table[fd].size - file_table[fd].open_offset;
	  res = ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, res);
    file_table[fd].open_offset += res;
	  return res;
  }  
	else {
		res = file_table[fd].write(buf, file_table[fd].open_offset, len);

    file_table[fd].open_offset += res;
	  return res;
  }
}