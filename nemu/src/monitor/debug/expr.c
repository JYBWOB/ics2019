#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

uint32_t isa_reg_str2val(const char *s, bool *success);

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_NUMBER, TK_REGISTER, TK_MINUS, TK_POINTOR,
  TK_NOTEQ, TK_AND, TK_OR, TK_HEX
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"\\$[a-zA-Z0-9]+", TK_REGISTER},
  {"0[xX][0-9a-fA-F]+", TK_NUMBER},  // hex
  {"0|[1-9][0-9]*", TK_NUMBER},
  {"!=", TK_NOTEQ},
  {"&&", TK_AND},
  {"\\|\\|", TK_OR},
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */

// 将正则表达式编译，写入re数组
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    // printf(rules[i].regex);
    // printf("\n");
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

// 优先级，为计算时提供方便
enum {
  OP_LV0 = 0, // number, register
  OP_LV1 = 10, // ()
  OP_LV2_1 = 21, // unary +, -
  OP_LV2_2 = 22, // deference *
  OP_LV3 = 30, // *, /, %
  OP_LV4 = 40, // +, -
  OP_LV7 = 70, // ==, !=
  OP_LV11 = 110, // &&
  OP_LV12 = 120, // ||
};


typedef struct token {
  int type;
  char str[32];
  int precedence;  // 为设置优先级，此处每个token都应有对应值
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        // 要给token设置优先级
        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case '+':
          case '-':
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].precedence = OP_LV4;
            ++nr_token;
            break;
          case '*':
          case '/':
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].precedence = OP_LV3;
            ++nr_token;
            break;
          case '(':
          case ')':
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].precedence = OP_LV1;
            ++nr_token;
            break;
          case TK_NUMBER:
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].precedence = OP_LV0;
            ++nr_token;
            break;
          case TK_REGISTER:
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].precedence = OP_LV0;
            ++nr_token;
            break;
          case TK_EQ:
          case TK_NOTEQ:
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].precedence = OP_LV7;
            ++nr_token;
            break;
          case TK_AND:
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].precedence = OP_LV11;
            ++nr_token;
            break;
          case TK_OR:
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].precedence = OP_LV12;
            ++nr_token;
            break;
          default:
            Log("Unhandled token found!\n");
            assert(0);
            break;
        }
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

bool check_parentheses(int p, int q, int* error) {
  if (p >= q) {
    *error = -1;
    return false;
  }
  int match_cnt = 0;
  for (int i = p; i <= q; ++i) {
    if (tokens[i].type == '(') {
      ++match_cnt;
    } else if (tokens[i].type == ')') {
      --match_cnt;
    }
    if (match_cnt < 0) {
      *error = -1;
      return false;
    }
  }
  if (match_cnt != 0) {
    *error = -1;
    return false;
  }
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    return true;
  } else {
    *error = 0;
    return false;
  }
}


uint32_t eval(int p, int q, bool *success) {
  int error;
  if (p > q) {
    *success = false;
    return 0;
  } else if (p == q) {
    uint32_t val;
    if (tokens[p].type == TK_NUMBER) {
      bool is_hex = strlen(tokens[p].str) > 2 && (tokens[p].str[1] == 'x' || tokens[p].str[1] == 'X');
      int ret;
      if (is_hex) {
        ret = sscanf(tokens[p].str, "%x", &val);
      } else {
        ret = sscanf(tokens[p].str, "%d", &val);
      }
      if (ret == 0) {
        *success = false;
      }
    } else if (tokens[p].type == TK_REGISTER) {
      val = isa_reg_str2val(tokens[p].str + 1, success);
    }
    return val;
  } else if (check_parentheses(p, q, &error)) {
    return eval(p + 1, q - 1, success);
  } else {
    if (error == -1) {
      *success = false;
      return 0;
    }
    int op = 0;
    int cur = 0;
    int max_precedence = -1;
    for (int i = p; i <= q; ++i) {
      if (tokens[i].type == '(') {
        ++cur;
      } else if (tokens[i].type == ')') {
        --cur;
      }
      if (cur == 0 && tokens[i].precedence >= max_precedence) {
        max_precedence = tokens[i].precedence;
        op = i;
      }
    }
    uint32_t val1 = 0;
    if (tokens[op].type != TK_POINTOR && tokens[op].type != TK_MINUS) {
      val1 = eval(p, op - 1, success);
    }
    uint32_t val2 = eval(op + 1, q, success);
    switch (tokens[op].type) {
      case '+':
        return val1 + val2;
        break;
      case '-':
        return val1 - val2;
        break;
      case '*':
        return val1 * val2;
        break;
      case '/':
        if (val2 == 0) {
          panic("Division by zero!!");
        }
        return val1 / val2;
        break;
      // TK_POINTOR and TK_MINUS do not support recursive expr evaluation
      case TK_POINTOR:
        return vaddr_read(val2, 4);
        break;
      case TK_MINUS:
        return -val2;
        break;
      case TK_EQ:
        return (val1 == val2);
        break;
      case TK_NOTEQ:
        return (val1 != val2);
        break;
      case TK_AND:
        return (val1 && val2);
        break;
      case TK_OR:
        return (val1 || val2);
        break;

      default:
        Log("Unhandled op %s\n", tokens[op].str);
        assert(0);
    }
  }
  return -1;
}

bool check_unary(int token_type) {
  return (
    token_type == '('
    || token_type == '+'
    || token_type == '-'
    || token_type == '*'
    || token_type == '/'
    || token_type == TK_MINUS
    || token_type == TK_POINTOR
  );
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for (int i = 0; i < nr_token; ++i) {
    if (tokens[i].type == '*' && (i == 0 || check_unary(tokens[i - 1].type))) {
      tokens[i].type = TK_POINTOR;
      tokens[i].precedence = OP_LV2_2;
    }
    if (tokens[i].type == '-' && (i == 0 || check_unary(tokens[i - 1].type))) {
      tokens[i].type = TK_MINUS;
      tokens[i].precedence = OP_LV2_1;
    }
  }
	*success = true;
	return eval(0, nr_token-1, success);
}
