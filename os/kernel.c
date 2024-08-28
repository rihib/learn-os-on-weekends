#include "kernel.h"
#include "common.h"

extern char __kernel_base[];
extern char __stack_top[];
extern char __bss[], __bss_end[];
extern char __free_ram[], __free_ram_end[];
extern char _binary_shell_bin_start[], _binary_shell_bin_size[];
paddr_t available_ram_start  = __free_ram;
uint32_t top_page_table[1024];   // 上位10ビットをページテーブルのインデックスとする

void kernel_main(void) {
  printf("Hello, RISC-V!\n");

  // メモリの初期化
  memset(__bss, 0, (size_t)__bss_end - (size_t)__bss);
  // トラップハンドラを設定
  WRITE_CSR(stvec,trap);
  
  printf("----------page table-----------\n");
  // ページテーブルの初期化
  for (int i = 0; i < 1024; i++) {
    top_page_table[i] = 0;
  }
  // カーネル空間の仮想アドレスと物理アドレスは同一のものにする
  for (uint32_t i = 0x80000000; i < 0x80000000 + 0x1000000; i += PAGE_SIZE) {
    page_mapping(top_page_table, i, i);
  }
  printf("----------page table end-----------\n");
  // ページテーブルのアドレスをsatpに設定s
  WRITE_CSR(satp, (uint32_t)top_page_table | SATP_SV32);


  // ここから本処理
  alloc_page_test();
  alloc_page_test();
  alloc_page_test();

  __asm__ __volatile__("unimp\n");
  PANIC("booted!");
}

__attribute__((section(".text.boot"))) __attribute__((naked)) void boot(void) {
  __asm__ __volatile__(
      "mv sp, %[stack_top]\n"
      "j kernel_main\n"
      :
      : [stack_top] "r"(__stack_top));
}

void trap(void) {
  // WRITE_CSR(sscratch, sp);
  __asm__ __volatile__(
    "csrw sscratch, sp;\n"
    "addi sp, sp, -120;\n"
    "sw ra, 0(sp);\n"
    "sw gp, -4(sp);\n"
    "sw tp, -8(sp);\n"
    "sw t0, -12(sp);\n"
    "sw t1, -16(sp);\n"
    "sw t2, -20(sp);\n"
    "sw t3, -24(sp);\n"
    "sw t4, -28(sp);\n"
    "sw t5, -32(sp);\n"
    "sw t6, -36(sp);\n"
    "sw a0, -40(sp);\n"
    "sw a1, -44(sp);\n"
    "sw a2, -48(sp);\n"
    "sw a3, -52(sp);\n"
    "sw a4, -56(sp);\n"
    "sw a5, -60(sp);\n"
    "sw a6, -64(sp);\n"
    "sw a7, -68(sp);\n"
    "sw s0, -72(sp);\n"
    "sw s1, -76(sp);\n"
    "sw s2, -80(sp);\n"
    "sw s3, -84(sp);\n"
    "sw s4, -88(sp);\n"
    "sw s5, -92(sp);\n"
    "sw s6, -96(sp);\n"
    "sw s7, -100(sp);\n"
    "sw s8, -104(sp);\n"
    "sw s9, -108(sp);\n"
    "sw s10, -112(sp);\n"
    "sw s11, -116(sp);\n"
  );
  uint32_t scause = READ_CSR(scause);
  uint32_t stval = READ_CSR(stval);
  uint32_t sepc = READ_CSR(sepc);
  uint32_t sstatus = READ_CSR(sstatus);

  printf("trap: scause=%x stval=%x sepc=%x sstatus=%x\n", scause, stval, sepc, sstatus);
  PANIC("trap!");

  // レジスタの値を戻す
  uint32_t sscratch = READ_CSR(sscratch);
  __asm__ __volatile__(
    "mv sp, %[sscratch];\n"::[sscratch] "r"(sscratch)
  );
  __asm__ __volatile__(
    "lw ra, 0(sp);\n"
    "lw gp, -4(sp);\n"
    "lw tp, -8(sp);\n"
    "lw t0, -12(sp);\n"
    "lw t1, -16(sp);\n"
    "lw t2, -20(sp);\n"
    "lw t3, -24(sp);\n"
    "lw t4, -28(sp);\n"
    "lw t5, -32(sp);\n"
    "lw t6, -36(sp);\n"
    "lw a0, -40(sp);\n"
    "lw a1, -44(sp);\n"
    "lw a2, -48(sp);\n"
    "lw a3, -52(sp);\n"
    "lw a4, -56(sp);\n"
    "lw a5, -60(sp);\n"
    "lw a6, -64(sp);\n"
    "lw a7, -68(sp);\n"
    "lw s0, -72(sp);\n"
    "lw s1, -76(sp);\n"
    "lw s2, -80(sp);\n"
    "lw s3, -84(sp);\n"
    "lw s4, -88(sp);\n"
    "lw s5, -92(sp);\n"
    "lw s6, -96(sp);\n"
    "lw s7, -100(sp);\n"
    "lw s8, -104(sp);\n"
    "lw s9, -108(sp);\n"
    "lw s10, -112(sp);\n"
    "lw s11, -116(sp);\n"
  );
  __asm__ __volatile__(
    "sret;\n"
  );
}

paddr_t alloc_page(int page_count) {
  size_t total_page_size = page_count * PAGE_SIZE;
  if (available_ram_start + total_page_size > __free_ram_end) {
    PANIC("out of memory");
  }
  memset(available_ram_start, 0, total_page_size);
  paddr_t ret = available_ram_start;
  available_ram_start += total_page_size;
  return ret;
}

void alloc_page_test(void) {
  printf("available_ram_start=%x\n", available_ram_start);
  paddr_t alloc_page_addr = alloc_page(1);
  printf("alloc_page_addr=%x\n", alloc_page_addr);
  printf("available_ram_start=%x\n", available_ram_start);
}

void page_mapping(uint32_t *first_page_table, uint32_t vaddr, uint32_t paddr) {
  printf("mapping vaddr=%x paddr=%x...\n", vaddr, paddr);
  if (is_aligned(vaddr, PAGE_SIZE) == false) {
    PANIC("vaddr is not aligned");
  }
  if (is_aligned(paddr, PAGE_SIZE) == false) {
    PANIC("paddr is not aligned");
  }
  uint32_t first_table_ind = (vaddr >> 22) & 0x3ff;
  uint32_t second_table_ind = (vaddr >> 12) & 0x3ff;
  uint32_t offset = (vaddr >> 0) & 0xfff;
  uint32_t second_table_info = first_page_table[first_table_ind];
  paddr_t second_table_paddr;
  if ((second_table_info & PAGE_V) == 0) {
    second_table_paddr = alloc_page(1);
    first_page_table[first_table_ind] = (second_table_paddr<<10) | PAGE_V | PAGE_R | PAGE_W | PAGE_X | PAGE_U;
    printf("second_table not made, made new table at %x\n", second_table_paddr);
  }
  else {
    second_table_paddr = (second_table_info & 0xfffffc00) >>10;
    printf("second_table found at %x\n", second_table_paddr);
  }
  uint32_t *second_page_table = second_table_paddr;
  second_page_table[second_table_ind] = (paddr << 10) | PAGE_V | PAGE_R | PAGE_W | PAGE_X | PAGE_U;
  printf("successfully mapped vaddr=%x paddr=%x\n", vaddr, paddr);
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
