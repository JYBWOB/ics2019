#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[300] = "";
static inline int gen_rand_expr() {
  if (strlen(buf) > 280) {
    return 0;
  }
  int result = 1;
  switch (rand() % 3) {
    case 0: {
      char temp[25]; 
      int ran = rand() % 100;
      sprintf(temp, "%d", ran);
      strcat(buf, temp);
      break;
    }
    case 1: { 
      strcat(buf, "(");
      result = gen_rand_expr(); 
      strcat(buf, ")");
      break;
    }
    default: {
      int resule1 = gen_rand_expr(); 
      int ran = rand() % 4;
      switch(ran) {
        case 0:
          strcat(buf, "+");
          break;
        case 1:
          strcat(buf, "-");
          break;
        case 2:
          strcat(buf, "*");
          break;
        default:
          strcat(buf, "/");
          break;
      } 
      int result2 = gen_rand_expr(); 
      result = resule1 && result2;
      break;
    }
  }
  return result;
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
    buf[0] = '\0';
    if(0 == gen_rand_expr()) {
      continue;
    }

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
