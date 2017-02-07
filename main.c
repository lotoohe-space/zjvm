#include <stdio.h>
#include <stdlib.h>
#define DEBUG_INSTRUCTION 1

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

#define STATCK_SIZE 2048
#define HEAP_SIZE 2048
/* 栈 */
u8 statck[STATCK_SIZE];
/* 堆栈位置 */
s32 statck_index=0;
/* 堆 */
u8 heap[HEAP_SIZE];
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

void item_dispose(u32 instruction) {
	u8 instruction_type;//指令的类型
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
			printf("mov R%d,%x\r\n",Rn,rn[Rn]);
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
		case 0x9:
		//pop(9)
		//--pop Rn
		//[31 - 27][26 - 24][23 - 0]
		//01001b xxxb x-- - xb
		//-- 0x48000000
		{
			u8 Rn = (instruction >> 24) & 0x7;
			if (statck_index < 0) {
				printf("%d-->statck error..\r\n", statck_index);
				exit(0);
			}
			//寄存器赋值常量
			rn[Rn] = statck[statck_index] ;
			statck_index--;
#if DEBUG_INSTRUCTION
			printf("pop R%d\r\n", Rn);
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
			//寄存器赋值常量
			rn[Rn0] = *((u32*)rn[Rn1]);
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
			//寄存器赋值常量
			*((u32*)rn[Rn1]) = rn[Rn0];
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
			break;
	}
}


int main(int argv, char *args[]) {
	int i = 0;
	u32 a = 0x123;
	rn[0] = 1;
	rn[1] = (u32)(&a);
	//for (i = 0; i < 2049; i++) {
	item_dispose(0x48000000);
	//}
	
	return 0;
}