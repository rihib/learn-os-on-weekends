#pragma once

#define PANIC(fmt, ...)                                                   \
  do {                                                                    \
    printf("PANIC: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    while (1) {                                                           \
    }                                                                     \
  } while (0)

#define READ_CSR(reg)                                     \
  ({                                                      \
    unsigned long __tmp;                                  \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                                                \
  })

#define WRITE_CSR(reg, value)                               \
  do {                                                      \
    uint32_t __tmp = (value);                               \
    __asm__ __volatile__("csrw " #reg ", %0" ::"r"(__tmp)); \
  } while (0)
