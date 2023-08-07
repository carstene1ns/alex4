
// ripped from Allegro-Legacy, under gift-ware license

#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <stdint.h>
#include <stdio.h>

typedef int32_t fixed;

extern fixed _cos_tbl[];

inline fixed ftofix(double x) {
   if (x > 32767.0) {
      printf("fixed math range error\n");
      return 0x7FFFFFFF;
   }

   if (x < -32767.0) {
      printf("fixed math range error\n");
      return -0x7FFFFFFF;
   }

   return (fixed)(x * 65536.0 + (x < 0 ? -0.5 : 0.5));
}

inline double fixtof(fixed x) {
   return (double)x / 65536.0;
}

inline fixed fixmul(fixed x, fixed y) {
  return ftofix(fixtof(x) * fixtof(y));
}

inline int fixfloor(fixed x) {
   /* (x >> 16) is not portable */
   if (x >= 0)
      return (x >> 16);
   else
      return ~((~x) >> 16);
}

inline fixed itofix(int x) {
   return x << 16;
}

inline int fixtoi(fixed x) {
   return fixfloor(x) + ((x & 0x8000) >> 15);
}

inline fixed fixcos(fixed x) {
   return _cos_tbl[((x + 0x4000) >> 15) & 0x1FF];
}

inline fixed fixsin(fixed x) {
   return _cos_tbl[((x - 0x400000 + 0x4000) >> 15) & 0x1FF];
}

#endif
