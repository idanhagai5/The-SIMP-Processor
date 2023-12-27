#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS

//define sizes
#define MEMORY_SIZE 512
#define MAX_INSTRUCTIONS 512
#define SIMP_REGS_SIZE 16
#define WRITE "WRITE"
#define READ "READ"

//initialize sizes

int32_t simp_regs[SIMP_REGS_SIZE] = { 0 };
int main_memory[MEMORY_SIZE] = { 0 };
int done = 0; 
int max_address = 0;


char* simp_regs_names[SIMP_REGS_SIZE] = { "$zero", "$imm", "$v0", "$a0", "$a1", "$t0", "$t1", "$t2","$t3", "$s0", \
"$s1", "$s2", "$gp", "$sp", "$fp", "$ra" };

char* functions_names[20] = { "add", "sub", "and", "or", "sll", "sra", "srl", "beq", "bne", "blt", "bgt", "ble", \
"bge", "jal", "lw", "sw", "reti", "in", "out", "halt" };

enum SIMP_regs_assign {
	$zero = 0, $imm = 1, $v0 = 2, $a0 = 3, $a1 = 4, $t0 = 5, $t1 = 6, $t2 = 7, $t3 = 8, $s0 = 9, $s1 = 10, $s2 = 11, \
	$gp = 12, $sp = 13, $fp = 14, $ra = 15,
};

//declare functions
void add(int* pc, int rd, int rs, int rt);
void sub(int* pc, int rd, int rs, int rt);
void and (int* pc, int rd, int rs, int rt);
void or (int* pc, int rd, int rs, int rt);
void sll(int* pc, int rd, int rs, int rt);
void sra(int* pc, int rd, int rs, int rt);
void srl(int* pc, int rd, int rs, int rt);
void beq(int* pc, int rd, int rs, int rt);
void bne(int* pc, int rd, int rs, int rt);
void blt(int* pc, int rd, int rs, int rt);
void bgt(int* pc, int rd, int rs, int rt);
void ble(int* pc, int rd, int rs, int rt);
void bge(int* pc, int rd, int rs, int rt);
void jal(int* pc, int rd, int rs, int rt);
void lw(int* pc, int rd, int rs, int rt);
void sw(int* pc, int rd, int rs, int rt);
void halt(int* pc, int rd, int rs, int rt);
int sign_extension(int imm);
void decode(int instruction, int* opcode, int* rd, int* rs, int* rt);
void excecute(int* pc, int opcode, int rd, int rs, int rt);
int choose_instruction(int* pc);
void simulator_run(FILE* memout, FILE* regout, FILE* trace, FILE* cycles);
void save_memin(FILE* memin);
void print_trace(FILE* trace, int* pc, int* instruction);
void print_files(int clks, FILE* cycles, FILE* memout, FILE* regout);
void find_max_written_address();



int main(int argc, char** argv)
{
	FILE* memin = NULL, * memout = NULL, * regout = NULL, * trace = NULL, * cycles = NULL;
	memin = fopen(argv[1], "r");
	if (memin == NULL) {
		printf("problem with opening memin file");
		goto Out;
	}
	memout = fopen(argv[2], "w");
	if (memout == NULL) {
		printf("problem with opening memout file");
		goto Out;
	}
	regout = fopen(argv[3], "w");
	if (regout == NULL) {
		printf("problem with opening regout file");
		goto Out;
	}
	trace = fopen(argv[4], "w");
	if (memin == NULL) {
		printf("problem with opening trace file");
		goto Out;
	}
	cycles = fopen(argv[5], "w");
	if (memin == NULL) {
		printf("problem with opening cycles file");
		goto Out;
	}


	save_memin(memin); //save instructions from memin file to main memory array
	simulator_run(memout, regout, trace, cycles);

Out:

	if (memin != NULL) {
		fclose(memin);
	}

	if (memout != NULL) {
		fclose(memout);
	}
	if (regout != NULL) {
		fclose(regout);
	}
	if (trace != NULL) {
		fclose(trace);
	}

	if (cycles != NULL) {
		fclose(cycles);
	}

	return 0;
}


//the run of the simulator
void simulator_run(FILE* memout, FILE* regout, FILE* trace, FILE* cycles) {
	int clks = 0;
	int opcode, rd, rs, rt;
	int inst = 0;
	int pc = 0;
	while (!done) {
		//choose the insturction
		inst = choose_instruction(&pc);
		//saves data of imm field in reg number 1
		simp_regs[$imm] = sign_extension(main_memory[pc] & 0xfff);
		print_trace(trace, &pc, &inst);
		//decode
		decode(inst, &opcode, &rd, &rs, &rt);
		//advance PC
		pc += 1;
		//execute
		excecute(&pc, opcode, rd, rs, rt);
		clks++;
	}
	find_max_written_address();
	print_files(clks, cycles, memout, regout);

}

//copy memory image from 'memin' to main memory array 
void save_memin(FILE* memin) {
	int i = 0;
	int content = 0;
	while (1 == fscanf(memin, "%x", &content)) {
		//content;
		main_memory[i] = content;
		i++;
	}
	max_address = i;
}


//extracted signed extend imm from a given instruction
int sign_extension(int imm) {
	int mask = 0x800;
	//checks if the imm is negetive or positive. if negative, sign extend with 1's
	// (imm is 12 bits long)
	if (mask & imm) {
		imm += 0xfffff000;
	}
	return imm;
}

// decoding current instruction
void decode(int  instruction, int* opcode, int* rd, int* rs, int* rt) {
	*opcode = (instruction >> 24) & 0xff;
	*rd = (instruction >> 20) & 0xf;
	*rs = (instruction >> 16) & 0xf;
	*rt = (instruction >> 12) & 0xf;

}

// executes instruction as function of opcode
void excecute(int* pc, int opcode, int rd, int rs, int rt) {
	switch (opcode) {
	case 0:
		if (rd != 0)
			add(pc, rd, rs, rt);
		break;
	case 1:
		if (rd != 0)
			sub(pc, rd, rs, rt);
		break;
	case 2:
		if (rd != 0)
			and (pc, rd, rs, rt);
		break;
	case 3:
		if (rd != 0)
			or (pc, rd, rs, rt);
		break;
	case 4:
		if (rd != 0)
			sll(pc, rd, rs, rt);
		break;
	case 5:
		if (rd != 0)
			sra(pc, rd, rs, rt);
		break;
	case 6:
		if (rd != 0)
			srl(pc, rd, rs, rt);
		break;
	case 7:
		beq(pc, rd, rs, rt);
		break;
	case 8:
		bne(pc, rd, rs, rt);
		break;
	case 9:
		blt(pc, rd, rs, rt);
		break;
	case 10:
		bgt(pc, rd, rs, rt);
		break;
	case 11:
		ble(pc, rd, rs, rt);
		break;
	case 12:
		bge(pc, rd, rs, rt);
		break;
	case 13:
		jal(pc, rd, rs, rt);
		break;
	case 14:
		if (rd != 0)
			lw(pc, rd, rs, rt);
		break;
	case 15:
		sw(pc, rd, rs, rt);
		break;
	case 19:
		halt(pc, rd, rs, rt);
		break;
	default:
		break;
	}
	return;
}

//chooses instruction to decode
int choose_instruction(int* pc)
{
	return main_memory[*pc];
}


void print_trace(FILE* trace, int* pc, int* instruction) {
	fprintf(trace, "%08X %08X", *pc, *instruction);
	//prints regs
	for (int i = 0; i < SIMP_REGS_SIZE; i++) {
		fprintf(trace, " %08X", simp_regs[i]);
	}
	fprintf(trace, "\n");
}

//prints files after simulator ends its run
void print_files(int clks,FILE* cycles, FILE* memout, FILE* regout) {
	//memout file
	for (int i = 0; i <= max_address; i++) {
		fprintf(memout, "%08X\n", main_memory[i]);
	}
	int i = 0;
	//regout file
	for (int i = 2; i < SIMP_REGS_SIZE; i++) {
		fprintf(regout, "%08X\n", simp_regs[i]);
	}

	//cycles file
	fprintf(cycles, "%d\n", clks);

}



// implemention of all the opcodes

// R[rd] = R[rs] + R[rt]
void add(int* pc, int rd, int rs, int rt) {
	simp_regs[rd] = simp_regs[rs] + simp_regs[rt];
}

// R[rd] = R[rs] - R[rt]
void sub(int* pc, int rd, int rs, int rt) {
	simp_regs[rd] = simp_regs[rs] - simp_regs[rt];
}

// R[rd] = R[rs] & R[rt]
void and (int* pc, int rd, int rs, int rt) {
	simp_regs[rd] = simp_regs[rs] & simp_regs[rt];
}

// R[rd] = R[rs] | R[rt]
void or (int* pc, int rd, int rs, int rt) {
	simp_regs[rd] = simp_regs[rs] | simp_regs[rt];
}

// R[rd] = R[rs] << R[rt]
void sll(int* pc, int rd, int rs, int rt) {
	simp_regs[rd] = simp_regs[rs] << simp_regs[rt];
}

//  R[rd] = R[rs] >> R[rt] (arithmetic shift)
void sra(int* pc, int rd, int rs, int rt) {
	if ((simp_regs[rs] < 0) && (simp_regs[rt] > 0)) {
		simp_regs[rd] = simp_regs[rs] >> simp_regs[rt] | ~(~0U >> simp_regs[rt]);
	}
	else {
		simp_regs[rd] = simp_regs[rs] >> simp_regs[rt];
	}
}

// R[rd] = R[rs] >> R[rt] (logic shift)
void srl(int* pc, int rd, int rs, int rt) {
	simp_regs[rd] = simp_regs[rs] >> simp_regs[rt];

}

//  if (R[rs] == R[rt]) @ pc = R[rd][low bits 11:0]
void beq(int* pc, int rd, int rs, int rt) {
	*pc = (simp_regs[rs] == simp_regs[rt]) ? (simp_regs[rd] & 0xfffu) % MEMORY_SIZE : *pc;
}

// if (R[rs] != R[rt]) @ pc = R[rd][low bits 11:0]
void bne(int* pc, int rd, int rs, int rt) {
	*pc = (simp_regs[rs] != simp_regs[rt]) ? (simp_regs[rd] & 0xfffu) % MEMORY_SIZE : *pc;
}

// if (R[rs] < R[rt]) @ pc = R[rd] [low bits 11:0]
void blt(int* pc, int rd, int rs, int rt) {
	*pc = (simp_regs[rs] < simp_regs[rt]) ? (simp_regs[rd] & 0xfffu) % MEMORY_SIZE : *pc;
}

//if (R[rs] > R[rt]) @ pc = R[rd] [low bits 11:0]
void bgt(int* pc, int rd, int rs, int rt) {
	*pc = (simp_regs[rs] > simp_regs[rt]) ? (simp_regs[rd] & 0xfffu) % MEMORY_SIZE : *pc;
}

//  if (R[rs] <= R[rt]) @ pc = R[rd] [low bits 11:0]
void ble(int* pc, int rd, int rs, int rt) {
	*pc = (simp_regs[rs] <= simp_regs[rt]) ? (simp_regs[rd] & 0xfffu) % MEMORY_SIZE : *pc;
}

//if (R[rs] >= R[rt]) @ pc = R[rd] [low bits 11:0]
void bge(int* pc, int rd, int rs, int rt) {
	*pc = (simp_regs[rs] >= simp_regs[rt]) ? (simp_regs[rd] & 0xfffu) % MEMORY_SIZE : *pc;
}

// pc = R[rd][11:0]
void jal(int* pc, int rd, int rs, int rt) {
	simp_regs[15] = *pc;
	*pc = (simp_regs[rd] & 0xfffu) % MEMORY_SIZE;
}

// R[rd] = memory[R[rs]+R[rt]]
void lw(int* pc, int rd, int rs, int rt) {
	if ((simp_regs[rs] + simp_regs[rt]) >= 0)
		simp_regs[rd] = main_memory[(simp_regs[rs] + simp_regs[rt]) % MEMORY_SIZE];
}

//  memory[R[rs]+R[rt]] = R[rd]
void sw(int* pc, int rd, int rs, int rt) {
	int address = (simp_regs[rs] + simp_regs[rt]) % MEMORY_SIZE;
	if ((address) >= 0)
		main_memory[address] = simp_regs[rd];
	if (address > max_address)
		max_address = address;


}

//end of assembly code
void halt(int* pc, int rd, int rs, int rt)
{
	done = 1;
}

// find max address in the main memory which is not empty;
void find_max_written_address()
{
	while (main_memory[max_address] == 0)
		max_address--;
}

