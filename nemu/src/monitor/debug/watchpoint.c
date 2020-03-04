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
  WP* ret = free_; // ret指向可用节点
  // 更新free
  free_ = free_->next;
  // 更新head
  ret->next = head;
  head = ret;
  return ret;
}

void free_wp(WP *wp) {
  assert(wp != NULL);
  assert(head != NULL);
  if(head == wp) {
    // 如果wp为第一个，则直接设置为空
    head = NULL;
  }
  else {
    // 否则逐个寻找
    WP* prev = head;
    while(prev->next != wp) {
      prev = prev->next;
    }
    // 链接跳过wp
    prev->next = wp->next;
  }
  // 更新free
  wp->next = free_;
  free_ = wp;
}

// 根据wp获取其NO值
WP* wp_no2ptr(int NO) {
  WP* ret = NULL;
  WP* curr = head;
  while (curr->NO != NO) {
    curr = curr->next;
  }
  ret = curr;
  return ret;
}

// 输出所有监视点的信息
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

// 检查各个监视点是否有变化
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