#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = "";
static inline void gen_rand_expr() {
  srand((unsigned)time(NULL));

  int index = 0;
  char temp[25]; 
  switch (choose(3)) {
    case 0:
      int ran = rand();
      itoa(ran, temp, 10);
      strcat(buf, temp);
      break;
    case 1: 
      strcat(buf, '(');
      gen_rand_expr(); 
      strcat(buf, ')');
      break;
    default:
      gen_rand_expr(); 
      int ran = rand() % 4;
      switch(ran) {
        case 0:
          strcat(buf, '+');
          break;
        case 1:
          strcat(buf, '-');
          break;
        case 2:
          strcat(buf, '*');
          break;
        default:
          strcat(buf, '/');
          break;
      } 
      gen_rand_expr(); 
      break;
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
