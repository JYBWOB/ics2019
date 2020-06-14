#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "assert.h"

typedef int FLOAT;

static inline int F_is_positive(FLOAT a) {
  return (a & 0x80000000) == 0;
}

static inline int F2int(FLOAT a) {
  // assert(0);
  int result = Fabs(a) / (1<<16);
  if(a & 0x80000000 != 0) {
    result |= 0x80000000;
  }
  return result;
}

static inline FLOAT int2F(int a) {
  // assert(0);
  int result = Fabs(a) * (1<<16);
  if(a < 0) 
    result = result & 0x80000000;
  return result;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
  // assert(0);
  int result = Fabs(a) * (b>=0?b:-b);
  if(F_is_positive(a) && b <0 || !F_is_positive(a) && b>0) {
    result |= 0x80000000;
  }
  return result;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
  // assert(0);
  int result = Fabs(a) / (b>=0?b:-b);
  if(F_is_positive(a) && b <0 || !F_is_positive(a) && b>0) {
    result |= 0x80000000;
  }
  return result;
}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT Fsqrt(FLOAT);
FLOAT Fpow(FLOAT, FLOAT);

#endif
