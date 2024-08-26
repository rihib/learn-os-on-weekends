#include "kernel.h"

#include "common.h"

extern char __kernel_base[];
extern char __stack_top[];
extern char __bss[], __bss_end[];
extern char __free_ram[], __free_ram_end[];
extern char _binary_shell_bin_start[], _binary_shell_bin_size[];
struct trap_frame trapflame;
__attribute__((aligned(4))) void traphandle(void){
  // save registers
  __asm__ __volatile__ (
    "csrw sscratch, a0\n"
    "la a0, trapflame\n"
    "sw ra, 0(a0)\n"
    "sw gp, 4(a0)\n"
    "sw tp, 8(a0)\n"
    "sw t0, 12(a0)\n"
    "sw t1, 16(a0)\n"
    "sw t2, 20(a0)\n"
    "sw t3, 24(a0)\n"
    "sw t4, 28(a0)\n"
    "sw t5, 32(a0)\n"
    "sw t6, 36(a0)\n"
    // "sw a0, 40(a0)\n" // a0 is used as a pointer to trapflame
    "sw a1, 44(a0)\n"
    "sw a2, 48(a0)\n"
    "sw a3, 52(a0)\n"
    "sw a4, 56(a0)\n"
    "sw a5, 60(a0)\n"
    "sw a6, 64(a0)\n"
    "sw a7, 68(a0)\n"
    "sw s0, 72(a0)\n"
    "sw s1, 76(a0)\n"
    "sw s2, 80(a0)\n"
    "sw s3, 84(a0)\n"
    "sw s4, 88(a0)\n"
    "sw s5, 92(a0)\n"
    "sw s6, 96(a0)\n"
    "sw s7, 100(a0)\n"
    "sw s8, 104(a0)\n"
    "sw s9, 108(a0)\n"
    "sw s10, 112(a0)\n"
    "sw s11, 116(a0)\n"
    "sw sp, 120(a0)\n"
    : 
    : 
    : "memory"
  );
  trapflame.a0 = READ_CSR(sscratch);
  printf("scause: %x\n", READ_CSR(SCAUSE));
  printf("sepc: %x\n", READ_CSR(SEPC));
  printf("stval: %x\n", READ_CSR(STVAL));
  PANIC("trap!");
  // restore registers
  WRITE_CSR(sscratch, trapflame.a0);
  __asm__ __volatile__ (
    "la a0, trapflame\n"
    "lw ra, a0\n"
    "lw gp, 4(a0)\n"
    "lw tp, 8(a0)\n"
    "lw t0, 12(a0)\n"
    "lw t1, 16(a0)\n"
    "lw t2, 20(a0)\n"
    "lw t3, 24(a0)\n"
    "lw t4, 28(a0)\n"
    "lw t5, 32(a0)\n"
    "lw t6, 36(a0)\n"
    // "lw a0, 40(a0)\n" // a0 is used as a pointer to trapflame
    "lw a1, 44(a0)\n"
    "lw a2, 48(a0)\n"
    "lw a3, 52(a0)\n"
    "lw a4, 56(a0)\n"
    "lw a5, 60(a0)\n"
    "lw a6, 64(a0)\n"
    "lw a7, 68(a0)\n"
    "lw s0, 72(a0)\n"
    "lw s1, 76(a0)\n"
    "lw s2, 80(a0)\n"
    "lw s3, 84(a0)\n"
    "lw s4, 88(a0)\n"
    "lw s5, 92(a0)\n"
    "lw s6, 96(a0)\n"
    "lw s7, 100(a0)\n"
    "lw s8, 104(a0)\n"
    "lw s9, 108(a0)\n"
    "lw s10, 112(a0)\n"
    "lw s11, 116(a0)\n"
    "lw sp, 120(a0)\n"
    "csrr a0, sscratch\n"
    : 
  );
  // return from trap
  __asm__ __volatile__ (
    "sret"
  );
}

void trapinit(void){
  WRITE_CSR(STVEC, traphandle);
}

void kernel_main(void) {
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
  trapinit();
  printf("trap initialized\n");
  __asm__ __volatile__("unimp");
  PANIC("booted!");
  printf("unreachable here!\n");
}

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n"
      "j kernel_main\n"
      :
      : [stack_top] "r"(__stack_top));
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
