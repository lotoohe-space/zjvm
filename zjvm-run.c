#include <stdio.h>
#include <stdlib.h>
#define DEBUG_INSTRUCTION 1

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

#define STATCK_SIZE 2048
#define HEAP_SIZE 2048
#define MEMORY_SIZE 2048
//内存
u32 memory[MEMORY_SIZE];
/* 栈 */
u32 statck[STATCK_SIZE];
/* 堆栈位置 */
s32 statck_index=0;
/* 堆 */
u32 heap[HEAP_SIZE];
/* r0 r1 r2(pc) r3(lr) */
u32 rn[4];// r0,r1,rn[2]-->pc,rn[3]-->lr
/*****************************************
32 bit 指令 
寄存器说明:
r0;r1;r2 (pc);r3 (lr)
mov (0-3)
	-- mov Rn, 0xffffffff
		[31-27][26-24][23-0]
		00000b	xxxb	x---xb
	-- mov Rn, Rn
		[31-27][26-24][23-21][20-0]
		00001b	xxxb	xxxb x---xb
push (4)
	-- push Rn
		[31-27][26-24][23-0]
		00100b xxxb   x---xb
	-- 0x20000000
add (5-8)
	-- add Rn, 0xffffffff
		[31-27][26-24][23-0]
		00000b	xxxb  x---xb
	-- add Rn, Rn 
		[31-27][26-24][23-21][20-0]
		00000b	xxxb  xxxb x---xb
pop (9)
	-- pop Rn
		[31-27][26-24][23-0]
		01001b xxxb   x---xb
	-- 0x48000000
mul (10-14)
	-- mul Rn, 0xffffffff
		0xa-0xf 0x0-0x7 000000000000000000000000b
	-- mul Rn, Rn
		0xa-0xf 0x0-0x7 0x0-0x7
ldr (14-18)
	-- ldr Rn,Rm
	[31-27][26-24][23-21][20-0]
	01101b	xxxb	xxxb x---xb
	-- 0x68200000
	-- Rm 中地址中的值复制到Rn中
str (19-23)
	-- str Rn,Rm
	[31-27][26-24][23-21][20-0]
	10011b	xxxb	xxxb x---xb
	-- 0x98200000
	-- Rn 中的值复制到Rm中的地址中去
*****************************************/

void item_dispose(u32 *data,int len) {
	u8 instruction_type;//指令的类型
	u32 instruction;//指令
	int i = 0;
	rn[2] = (u32)data;
	for (i=0;i<len;i++) {
		instruction = *(unsigned int*)(rn[2]);
		rn[2] += 4;
		instruction_type = (instruction >> 27) & 0x1f;
		switch (instruction_type) {
			//mov指令
		case 0x0:
			//常量到寄存器
		{
			//--mov Rn, 0xffffffff
			//[31 - 27][26 - 24][23 - 0]
			//00000b	xxxb	x-- - xb
			u8 Rn = (instruction >> 24) & 0x7;
			//寄存器赋值常量
			rn[Rn] = instruction & 0xffffff;
#if DEBUG_INSTRUCTION
			printf("mov R%d,%x\r\n", Rn, rn[Rn]);
#endif
		}
		break;
		case 0x1:
			//寄存器到寄存器
		{
			//--mov Rn, Rn
			//	[31 - 27][26 - 24][23 - 21][20 - 0]
			//	00001b	xxxb	xxxb x-- - xb
			u8 Rn0 = (instruction >> 24) & 0x7;
			u8 Rn1 = (instruction >> 21) & 0x7;
			//寄存器赋值常量
			rn[Rn0] = rn[Rn1];
#if DEBUG_INSTRUCTION
			printf("mov R%d,R%d\r\n", Rn0, Rn1);
#endif
		}
		break;
		case 0x2:
			break;
		case 0x3:
			break;
		case 0x4:
			//push(4)
			//--push Rn
			//[31 - 27][26 - 24][23 - 0]
			//00100b xxxb x-- - xb
			//-- 0x20000000
		{
			u8 Rn = (instruction >> 24) & 0x7;
			if (statck_index >= STATCK_SIZE) {
				printf("%d-->statck error..\r\n", statck_index);
				exit(0);
			}
			//寄存器赋值常量
			statck[statck_index] = rn[Rn];
			statck_index++;
#if DEBUG_INSTRUCTION
			printf("push R%d\r\n", Rn);
#endif
		}
		break;
		case 0x5:
			//add(5 - 8)
			//--add Rn, 0xffffffff
			//[31 - 27][26 - 24][23 - 0]
			//00000b	xxxb  x-- - xb
		{
			u8 Rn = (instruction >> 24) & 0x7;
			//寄存器赋值常量
			rn[Rn] += instruction & 0xffffff;
#if DEBUG_INSTRUCTION
			printf("add R%d,%x\r\n", Rn, instruction & 0xffffff);
#endif
		}
		break;
		case 0x6:
			//add(5 - 8)
			//-- add Rn, Rn
			//[31 - 27][26 - 24][23 - 21][20 - 0]
			//00000b	xxxb  xxxb x-- - xb
		{
			u8 Rn0 = (instruction >> 24) & 0x7;
			u8 Rn1 = (instruction >> 21) & 0x7;
			//寄存器赋值常量
			rn[Rn0] += rn[Rn1];
#if DEBUG_INSTRUCTION
			printf("add R%d,R%d\r\n", Rn0, Rn1);
#endif
		}
		break;
		case 0x9:
			//pop(9)
			//--pop Rn
			//[31 - 27][26 - 24][23 - 0]
			//01001b xxxb x-- - xb
			//-- 0x48000000
		{
			u8 Rn = (instruction >> 24) & 0x7;	
			statck_index--;
			if (statck_index < 0) {
				printf("%d-->statck error..\r\n", statck_index);
				exit(0);
			}
			//寄存器赋值常量
			rn[Rn] = statck[statck_index];
			
#if DEBUG_INSTRUCTION
			printf("pop R%d\r\n", Rn);
#endif
		}
		break;
		case 10:
			//mul(10 - 14)
			//--mul Rn, 0xffffffff
			//0xa - 0xf 0x0 - 0x7 000000000000000000000000b
		{
			u8 Rn = (instruction >> 24) & 0x7;
			//寄存器赋值常量
			rn[Rn] *= instruction & 0xffffff;
#if DEBUG_INSTRUCTION
			printf("mul R%d,%x\r\n", Rn, instruction & 0xffffff);
#endif
		}
		break;
		case 11:
			//mul(10 - 14)
			//-- mul Rn, Rn
			//0xa - 0xf 0x0 - 0x7 0x0 - 0x7
		{
			u8 Rn0 = (instruction >> 24) & 0x7;
			u8 Rn1 = (instruction >> 21) & 0x7;
			//寄存器赋值常量
			rn[Rn0] *= rn[Rn1];
#if DEBUG_INSTRUCTION
			printf("mul R%d,R%d\r\n", Rn0, Rn1);
#endif
		}
		break;
		case 14:
			//ldr(14 - 17)
			//--ldr Rn, Rm
			//[31 - 27][26 - 24][23 - 21][20 - 0]
			//01110b	xxxb	xxxb x-- - xb
			//-- Rm 中地址中的值复制到Rn中
		{
			u8 Rn0 = (instruction >> 24) & 0x7;
			u8 Rn1 = (instruction >> 21) & 0x7;
			if (rn[Rn1] + memory > memory + MEMORY_SIZE - 1) {
				printf("memory error..\n");
				exit(0);
			}
			//寄存器赋值常量
			rn[Rn0] = *((u32*)(rn[Rn1] + memory));
#if DEBUG_INSTRUCTION
			printf("ldr R%d,R%d\r\n", Rn0, Rn1);
#endif		
		}
		break;
		case 15:
			break;
		case 16:
			break;
		case 17:
			break;
		case 18:
			break;

		case 19:
			//str(19 - 23)
			//--str Rn, Rm
			//[31 - 27][26 - 24][23 - 21][20 - 0]
			//10001b	xxxb	xxxb x-- - xb
			//-- Rn 中的值复制到Rm中的地址中去
		{
			u8 Rn0 = (instruction >> 24) & 0x7;
			u8 Rn1 = (instruction >> 21) & 0x7;
			if (rn[Rn1] + memory > memory + MEMORY_SIZE - 1) {
				printf("memory error..\n");
				exit(0);
			}
			//寄存器赋值常量
			*((u32*)(memory + rn[Rn1])) = rn[Rn0];
#if DEBUG_INSTRUCTION
			printf("str R%d,R%d\r\n", Rn0, Rn1);
#endif		
		}
		break;
		case 20:
			break;
		case 21:
			break;
		default:
			printf("instruction error..\r\n");
			exit(0);
		}
	}
}


int main(int argv, char *args[]) {
	int i = 0;
	u32 a = 0x123;
	rn[0] = 1;
	rn[1] = (u32)(&a);
	char *data;
	int len;
	FILE *fp = fopen("c:/asm.txt", "rb");
	if (fp == NULL) { return; }
	fseek(fp, 0, SEEK_END);
	len=ftell(fp);
	data = (char*)malloc(len+1);
	fseek(fp, 0, SEEK_SET);
	fread(data, len, 1, fp);
	fclose(fp);

	item_dispose(((unsigned int *)data), (len / sizeof(unsigned int)));

	printf("r0->%xH\nr1->%xH\npc->%xH\nlr->%xH\n", rn[0], rn[1], rn[2], rn[3]);
	return 0;
}