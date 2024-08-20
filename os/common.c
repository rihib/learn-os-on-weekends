#include "common.h"

void *memset(void *buf, char c, size_t n) {
  uint8_t *p = (uint8_t *)buf;
  while (n--) *p++ = c;
  return buf;
}

void *memcpy(void *dst, const void *src, size_t n) {
  uint8_t *d = (uint8_t *)dst;
  const uint8_t *s = (const uint8_t *)src;
  while (n--) *d++ = *s++;
  return dst;
}

char *strcpy(char *dst, const char *src) {
  char *d = dst;
  while (*src) *d++ = *src++;
  *d = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2) break;
    s1++;
    s2++;
  }

  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                       long arg5, long fid, long eid) {
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid;
  register long a7 __asm__("a7") = eid;

  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)
                       : "memory");
  return (struct sbiret){.error = a0, .value = a1};
}

void putchar(char ch) {
  sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
}

void printf(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  while (*fmt) {
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
        case '\0':
          putchar('%');
          goto end;
        case '%':
          putchar('%');
          break;
        case 's': {
          const char *s = va_arg(vargs, const char *);
          while (*s) {
            putchar(*s);
            s++;
          }
          break;
        }
        case 'd': {
          int value = va_arg(vargs, int);
          if (value < 0) {
            putchar('-');
            value = -value;
          }

          int divisor = 1;
          while (value / divisor > 9) divisor *= 10;

          while (divisor > 0) {
            putchar('0' + value / divisor);
            value %= divisor;
            divisor /= 10;
          }

          break;
        }
        case 'x': {
          int value = va_arg(vargs, int);
          for (int i = 7; i >= 0; i--) {
            int nibble = (value >> (i * 4)) & 0xf;
            putchar("0123456789abcdef"[nibble]);
          }
        }
      }
    } else {
      putchar(*fmt);
    }

    fmt++;
  }

end:
  va_end(vargs);
}
