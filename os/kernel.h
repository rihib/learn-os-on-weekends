#pragma once

#define PROCS_MAX 8           // 最大プロセス数
#define PROC_UNUSED 0         // 未使用のプロセス管理構造体
#define PROC_RUNNABLE 1       // 実行可能なプロセス
#define SATP_SV32 (1u << 31)  // Sv32モードでページングを有効化
#define PAGE_V (1 << 0)       // 有効化ビット
#define PAGE_R (1 << 1)       // 読み込み可能
#define PAGE_W (1 << 2)       // 書き込み可能
#define PAGE_X (1 << 3)       // 実行可能
#define PAGE_U (1 << 4)       // ユーザーモードでアクセス可能

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
