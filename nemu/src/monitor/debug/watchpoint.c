#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  assert(free_ != NULL);
  WP* ret = free_;
  free_ = free_->next;
  ret->next = head;
  head = ret;
  return ret;
}

void free_wp(WP *wp) {
  assert(wp != NULL);
  assert(head != NULL);
  if(head == wp) {
    head = NULL;
  }
  else {
    WP* prev = head;
    while(prev->next != wp) {
      prev = prev->next;
    }
    prev->next = wp->next;
  }
  wp->next = free_;
  free_ = wp;
}

WP* wp_no2ptr(int NO) {
  WP* ret = NULL;
  WP* curr = head;
  while (curr->NO != NO) {
    curr = curr->next;
  }
  ret = curr;
  return ret;
}

void wp_display() {
  WP* curr = head;
  if (curr == NULL) {
    printf("No watchpoints.\n");
    return;
  }
  printf("Num\tExpr\tValue\n");
  while (curr) {
    printf("%d\t%s\t0x%08x\n", curr->NO, curr->expression, curr->val);
    curr = curr->next;
  }
}

bool wp_check() {
  WP* curr = head;
  bool success;
  uint32_t new_val;
  while (curr) {
    success = true;
    new_val = expr(curr->expression, &success);
    assert(success != false);
    if (new_val != curr->val) {
      printf("wp %d\t%s = 0x%08x != 0x%08x\n", curr->NO, curr->expression, curr->val, new_val);
      return true;
    }
    curr = curr->next;
  }
  return false;
}