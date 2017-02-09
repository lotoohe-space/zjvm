#include <stdio.h>
#include <string.h>
#include <math.h>
#define DEF_MOV0 0
#define DEF_PUSH 4
#define DEF_ADD0 5
#define DEF_POP  9
#define DEF_MUL0 10
#define DEF_LDR 14
#define DEF_STR 19

unsigned int hexStr2Dim(char *hex) {
	int len = strlen(hex);
	int i;
	int count = 0;
	unsigned int num = 0;
	for (i = len - 1; hex[i]; i--) {
		if (hex[i] >= '0'&&hex[i] <= '9') {
			num += count ? (hex[i] - '0') * pow(16, count) : (hex[i] - '0');
			count++;
		}
		else if (hex[i] >= 'A'&&hex[i] <= 'F') {
			num += count ? (hex[i] - 'A') *  pow(16, count) : (hex[i] - 'A');
			count++;
		}
		else if (hex[i] >= 'a'&&hex[i] <= 'f') {
			num += count ? (hex[i] - 'a') *  pow(16, count) : (hex[i] - 'a');
			count++;
		}

	}
	return num;
}


//编译代码
//@param code 代码的文本
void compile(char *file) {
	char buffer[2048];
	char *str_temp;

	FILE *fp;
	FILE *wfp;
	fp = fopen(file, "r");
	if (fp == NULL) { return; }
	wfp = fopen("c:/asm.txt", "wb+");
	if (wfp == NULL) { fclose(wfp); return; }
	while (fgets(buffer, 2048, fp) != NULL) {
		//写入的字
		unsigned int write_word = 0;
		//指令类型
		unsigned char asn_type;
		int i = 0;
		buffer[strlen(buffer) - 1] = 0x00;
		do {
			if (i == 0) {
				str_temp = strtok(buffer, " ");
				i = 1;
			}
			else {
				str_temp = strtok(NULL, " ");
				i++;
			}
			if (str_temp == NULL) { break; }
			switch (i) {
			case 1:
				printf("%s\n", str_temp);
				if (strcmp("mov", str_temp) == 0) {
					//数据转移指令
					asn_type = DEF_MOV0;
					write_word = 1 << 27;
				}
				else if (strcmp("add", str_temp) == 0) {
					//加法指令
					asn_type = DEF_ADD0;
					write_word = 6 << 27;
				}
				else if (strcmp("mul", str_temp) == 0) {
					//乘法指令
					asn_type = DEF_MUL0;
					write_word = 11 << 27;
				}
				else if (strcmp("ldr", str_temp) == 0) {
					//地址数据传送指令
					asn_type = DEF_LDR;
					write_word = 14 << 27;
				}
				else if (strcmp("str", str_temp) == 0) {
					//数据到地址传送指令
					asn_type = DEF_STR;
					write_word = 19 << 27;
				}
				else if (strcmp("pop", str_temp) == 0) {
					//压栈指令
					asn_type = DEF_POP;
					write_word = 9 << 27;
				}
				else if (strcmp("push", str_temp) == 0) {
					//出栈指令
					asn_type = DEF_PUSH;
					write_word = 4 << 27;
				}
				break;
			case 2:
				if (strcmp("r0", str_temp) == 0) {
					write_word |= 0 << 24;
				}
				else if (strcmp("r1", str_temp) == 0) {
					write_word |= 1 << 24;
				}
				else if (strcmp("r2", str_temp) == 0 || strcmp("pc", str_temp) == 0) {
					write_word |= 2 << 24;
				}
				else if (strcmp("r3", str_temp) == 0 || strcmp("lr", str_temp) == 0) {
					write_word |= 3 << 24;
				}
				if (asn_type == DEF_POP || asn_type == DEF_PUSH) {
					goto write_bin;
				}
				break;
			case 3:
				if (strcmp("r0", str_temp) == 0) {
					write_word |= 0 << 21;
				}
				else if (strcmp("r1", str_temp) == 0) {
					write_word |= 1 << 21;
				}
				else if (strcmp("r2", str_temp) == 0 || strcmp("pc", str_temp) == 0) {
					write_word |= 2 << 21;
				}
				else if (strcmp("r3", str_temp) == 0 || strcmp("lr", str_temp) == 0) {
					write_word |= 3 << 21;
				}
				else {
					//str_temp[strlen(str_temp) - 1] = 0x00;
					//16进制常数
					write_word |= 0xffffff & hexStr2Dim(str_temp);
					switch (asn_type) {
					case DEF_MOV0:
						write_word = write_word & 0x7ffffff | (0 << 27);
						break;
					case DEF_ADD0:
						write_word = write_word & 0x7ffffff | (5 << 27);
						break;
					case DEF_MUL0:
						write_word = write_word & 0x7ffffff | (10 << 27);
						break;
					}
				}

				break;
			}
			if (i > 4) {
				break;
			}
		} while (1);
	write_bin:
		fwrite(&write_word, sizeof(unsigned int), 1, wfp);
	}

	fclose(wfp);
	fclose(fp);
}



int main(void) {

	compile("C:\\jvm_asm.txt");
	return 0;
}