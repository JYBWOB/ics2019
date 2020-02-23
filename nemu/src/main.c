int init_monitor(int, char *[]);
void ui_mainloop(int);

#include "monitor/debug/expr.c"

int main(int argc, char *argv[]) {

  bool *e = false;
  printf("%d\n", expr("(2056810607+(311588048)*((155358798/(1037229738)))*((109528855)))", e));

  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
