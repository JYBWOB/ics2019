#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expression[64];   // 表达式
  uint32_t val;          // 表达式计算值
} WP;

#endif

WP* new_wp();
void free_wp(WP *wp);
WP* wp_no2ptr(int NO);
void wp_display();
bool wp_check();