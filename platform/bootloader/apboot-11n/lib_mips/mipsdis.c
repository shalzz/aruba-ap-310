/*
** mipsdis.c -- disassemble MIPS instructions
*/

#include "common.h"
#if !defined(CONFIG_STAGE1) && !defined(CONFIG_STAGE2)
#include "command.h"
#include "watchdog.h"

static void dis_reg1(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d", opcode,
          ((instr >> 21) & 0x1f));
}

static void dis_reg12(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,$%d", opcode,
          ((instr >> 21) & 0x1f),
          ((instr >> 16) & 0x1f));
}

static void dis_reg13(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,$%d", opcode,
          ((instr >> 21) & 0x1f),
          ((instr >> 11) & 0x1f));
}

static void dis_reg123(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,$%d,$%d", opcode,
          ((instr >> 11) & 0x1f),
          ((instr >> 16) & 0x1f),
          ((instr >> 21) & 0x1f));
}

static void dis_reg3(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s \t$%d", opcode,
          ((instr >> 11) & 0x1f));
}

static void dis_reg12shift(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,$%d,%d", opcode,
          ((instr >> 11) & 0x1f),
          ((instr >> 16) & 0x1f),
          ((instr >>  6) & 0x1f));
}

static void dis_reg12br(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,$%d,0x%x", opcode,
          ((instr >> 16) & 0x1f),
          ((instr >> 21) & 0x1f),
          (instr & 0xffff));
}

static void dis_reg1br(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,0x%x", opcode,
          ((instr >> 21) & 0x1f),
          (instr & 0xffff));
}

static void dis_reg2off(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,0x%x", opcode,
          ((instr >> 16) & 0x1f),
          (instr & 0xffff));
}

static void dis_reg12off(char *buf, unsigned long instr, char *opcode)
{
  sprintf(buf, "%s\t$%d,0x%x($%d)", opcode,
          ((instr >> 16) & 0x1f),
          (instr & 0xffff),
          ((instr >> 21) & 0x1f));
}

char *disassemble(unsigned long instr)
{
  static char asmstr[32];
  switch ((instr >> 26) & 0x3f) {
  case 0:
    switch(instr & 0x3f) {
    case 0:
      if (instr) {
        dis_reg12shift(asmstr, instr, "sll");
      } else {
        sprintf(asmstr, "nop");
      }
      break;
    case 2:
      dis_reg12shift(asmstr, instr, "srl");
      break;
    case 3:
      dis_reg12shift(asmstr, instr, "sra");
      break;
    case 4:
      dis_reg123(asmstr, instr, "sllv");
      break;
    case 5:
      break;
    case 6:
      dis_reg123(asmstr, instr, "srlv");
      break;
    case 7:
      dis_reg123(asmstr, instr, "srav");
      break;
    case 8:
      dis_reg1(asmstr, instr, "jr");
      break;
    case 9:
      dis_reg13(asmstr, instr, "jalr");
      break;
    case 10:
      dis_reg123(asmstr, instr, "movz");
      break;
    case 11:
      dis_reg123(asmstr, instr, "movn");
      break;
    case 12:
      sprintf(asmstr, "syscall");
      break;
    case 13:
      sprintf(asmstr, "break");
      break;
    case 15:
      sprintf(asmstr, "sync");
      break;
    case 16:
      dis_reg3(asmstr, instr, "mfhi");
      break;
    case 17:
      dis_reg1(asmstr, instr, "mthi");
      break;
    case 18:
      dis_reg3(asmstr, instr, "mflo");
      break;
    case 19:
      dis_reg1(asmstr, instr, "mtlo");
      break;
    case 24:
      dis_reg12(asmstr, instr, "mult");
      break;
    case 25:
      dis_reg12(asmstr, instr, "multu");
      break;
    case 26:
      dis_reg12(asmstr, instr, "divu");
      break;
    case 27:
      dis_reg12(asmstr, instr, "divu");
      break;
    case 32:
      dis_reg123(asmstr, instr, "add");
      break;
    case 33:
      dis_reg123(asmstr, instr, "addu");
      break;
    case 34:
      dis_reg123(asmstr, instr, "sub");
      break;
    case 35:
      dis_reg123(asmstr, instr, "subu");
      break;
    case 36:
      dis_reg123(asmstr, instr, "and");
      break;
    case 37:
      dis_reg123(asmstr, instr, "or");
      break;
    case 38:
      dis_reg123(asmstr, instr, "xor");
      break;
    case 39:
      dis_reg123(asmstr, instr, "nor");
      break;
    case 42:
      dis_reg123(asmstr, instr, "slt");
      break;
    case 43:
      dis_reg123(asmstr, instr, "sltu");
      break;
    default:
      sprintf(asmstr, "unknown");
    }
    break;
  case 1:
    switch ((instr >> 16) & 0x1f) {
    case 0:
      dis_reg1br(asmstr, instr, "bltz");
      break;
    case 1:
      dis_reg1br(asmstr, instr, "bgez");
      break;
    case 2:
      dis_reg1br(asmstr, instr, "bltzl");
      break;
    case 3:
      dis_reg1br(asmstr, instr, "bgezl");
      break;
    default:
      sprintf(asmstr, "unknown");
    }
    break;
  case 2:
    sprintf(asmstr, "j\t0x%x", (instr & 0x3ffffff) << 2);
    break;
  case 3:
    sprintf(asmstr, "jal\t0x%x", (instr & 0x3ffffff) << 2);
    break;
  case 4:
    dis_reg12br(asmstr, instr, "beq");
    break;
  case 5:
    dis_reg12br(asmstr, instr, "bne");
    break;
  case 6:
    dis_reg1br(asmstr, instr, "blez");
    break;
  case 7:
    dis_reg1br(asmstr, instr, "bgtz");
    break;
  case 8:
    dis_reg12br(asmstr, instr, "addi");
    break;
  case 9:
    dis_reg12br(asmstr, instr, "addiu");
    break;
  case 10:
    dis_reg12br(asmstr, instr, "slti");
    break;
  case 11:
    dis_reg12br(asmstr, instr, "sltiu");
    break;
  case 12:
    dis_reg12br(asmstr, instr, "andi");
    break;
  case 13:
    dis_reg12br(asmstr, instr, "ori");
    break;
  case 14:
    dis_reg12br(asmstr, instr, "xori");
    break;
  case 15:
    dis_reg2off(asmstr, instr, "lui");
    break;
  case 20:
    dis_reg12br(asmstr, instr, "beql");
    break;
  case 21:
    dis_reg12br(asmstr, instr, "bnel");
    break;
  case 22:
    dis_reg12br(asmstr, instr, "blezl");
    break;
  case 23:
    dis_reg12br(asmstr, instr, "bgtzl");
    break;
  case 32:
    dis_reg12off(asmstr, instr, "lb");
    break;
  case 33:
    dis_reg12off(asmstr, instr, "lh");
    break;
  case 34:
    dis_reg12off(asmstr, instr, "lwl");
    break;
  case 35:
    dis_reg12off(asmstr, instr, "lw");
    break;
  case 36:
    dis_reg12off(asmstr, instr, "lbu");
    break;
  case 37:
    dis_reg12off(asmstr, instr, "lhu");
    break;
  case 38:
    dis_reg12off(asmstr, instr, "lwr");
    break;
  case 39:
    dis_reg12off(asmstr, instr, "lwu");
    break;
  case 40:
    dis_reg12off(asmstr, instr, "sb");
    break;
  case 41:
    dis_reg12off(asmstr, instr, "sh");
    break;
  case 42:
    dis_reg12off(asmstr, instr, "swl");
    break;
  case 43:
    dis_reg12off(asmstr, instr, "sw");
    break;
  case 46:
    dis_reg12off(asmstr, instr, "swr");
    break;
  default:
      sprintf(asmstr, "unknown");
      break;
  }
  return asmstr;
}

int 
do_dis(cmd_tbl_t *cmdtp, int flag, int argc, char **argv)
{
  unsigned int *addr = NULL;
  unsigned int cnt = 1;
  unsigned int i;
  unsigned int data;

  if (argc < 2) {
    return 1;
  }

  addr = (unsigned int *)simple_strtoul(argv[1], 0, 16);
  if (addr == 0)
    return 1;
  if ((unsigned int) addr & 0x3) {
    printf("Address is not aligned\n");
    return 1;
  }
#if 0
  if ((K0_TO_PHYS(addr) >= (DRAMSZ<<20)) &&
      (addr < (unsigned int *)CFG_FLASH_BASE)) {
    printf("Address is outside of physical memory\n");
    return 1;
  }
#endif
  if (argc > 2) {
    cnt = simple_strtoul(argv[2], 0, 16);
  }
#if 0
  addr = (unsigned int *)PHYS_TO_K1(addr);
#endif
  for (i = 0; i < cnt; i++) {
    data = *addr;
    printf("%08x:  %08x\t%s\n", addr, data, disassemble(data));
    WATCHDOG_RESET();
    addr++;
  }
  return 0;
}

U_BOOT_CMD(
	dis,	3,	1,	do_dis,
	"dis - disassemble MIPS instructions\n",
	"disassemble MIPS instructions\n"
);
#endif
