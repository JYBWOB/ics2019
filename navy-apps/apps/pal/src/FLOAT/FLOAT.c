#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

static inline int F_is_positive(FLOAT a);

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  // assert(0);
  FLOAT result = (Fabs(a) >> 8) * (Fabs(b) >> 8);
  if(F_is_positive(a) && !F_is_positive(b) || !F_is_positive(a) && F_is_positive(b)) {
    result |= 0x80000000;
  }  
  return result;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  // assert(0);
  FLOAT result = Fabs(a) / Fabs(b) * (1<<16);
  if(F_is_positive(a) && !F_is_positive(b) || !F_is_positive(a) && F_is_positive(b)) {
    result |= 0x80000000;
  }  
  return result;
}

// 此函数参考1711352 李煦阳同学的代码
FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

  // assert(0);
  uint32_t ui = *(uint32_t*)&a;
  // 符号位
  uint32_t sign = ui >> 31;
  // 指数
  uint32_t exp = (ui >> 23) & 0xff;
  int res = ui & 0x7fffff; // 取fraction
  // 浮点数表示中省略了一个1
  if (exp != 0)  
      res += 1 << 23;
  exp -= 150; // 将re有效位完全以整数看待, 算出它的2指数
  if (exp < -16)  // 以16为分界, 找到移位即可.
      res >>= -16 - exp;  // 去掉低于16位的位数
  if (exp > -16)
      res <<= exp + 16;  // 没有考虑溢出. 
  return sign == 0 ? res : -res;
}

FLOAT Fabs(FLOAT a) {
  // assert(0);
  a &= 0x7fffffff;
  return a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
