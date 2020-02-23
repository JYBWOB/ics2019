#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void isa_reg_display();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_scan(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si", "Execute N instructions, the default number is 1", cmd_si},
  {"info", "Print the regsters or watchpoints", cmd_info},
  {"x", "Scan the memory to find the value", cmd_scan},
};

/* 单步执行si */
static int cmd_si(char *args) {
  // 没有参数，则默认为1步
  if(args == NULL) {
    cpu_exec(1);
    return 0;
  }
  // 有参数，获取
  int n;
  if(sscanf(args, "%d", &n) == EOF) {
    printf("please input a number as args\n");
  }
  else {
    cpu_exec(n);
  }
  return 0;
}
/* 打印寄存器/监视点状态 */
static int cmd_info(char *args) {
  // 没有参数，则默认打印寄存器信息
  if(args == NULL) {
    isa_reg_display();
    return 0;
  }
  switch(*args) {
    case 'r':
      isa_reg_display();
      break;
    case 'w':
      printf("Will be implemented in pa1.3\n");
      break;
    default:
      printf("Unknown Type to print\n");
      break;
  }
  return 0;
}

// 扫描内存
static int cmd_scan(char *args) {
  char *scan_num_str = strtok(NULL, " ");
  if (scan_num_str == NULL) {
    printf("invalid args\nusage: x N address\n");
    return 0;
  }
  int scan_num = atoi(scan_num_str);
  char *expression = scan_num_str + strlen(scan_num_str) + 1;
  if (expression == NULL) {
    printf("invalid args\nusage: x N address\n");
    return 0;
  }
  uint32_t expr_val;
  bool success = true;
  expr_val = expr(expression, &success);
  if (success == false) {
    Log("Expr evaluation failed.");
    assert(0);
  }
  uint32_t val;
  for (int i = 0; i < scan_num; ++i) {
    val = vaddr_read(expr_val + (i << 2), 4);
    printf("%08x ", val);
  }
  printf("\n");
  return 0;
}

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
