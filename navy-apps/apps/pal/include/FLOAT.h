#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "assert.h"

typedef int FLOAT;

static inline int F2int(FLOAT a) {
  // assert(0);
  int result = a >> 16;
  return result;
}

static inline FLOAT int2F(int a) {
  // assert(0);
  FLOAT result = a << 16;
  return result;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
  // assert(0);
  return a * b;
  return 0;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
  // assert(0);
  return a / b;
  return 0;
}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT Fsqrt(FLOAT);
FLOAT Fpow(FLOAT, FLOAT);

#endif
