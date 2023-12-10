# The SIMP Processor
In this project, I developed both an assembler and a simulator as separate programs for a RISC processor known as SIMP.
Additionally, I authored assembly language programs tailored for the SIMP architecture. The simulator executes each insrtuction within a single clock cycle. 

## registers 
The SIMP processor comprises 16 registers, each 32 bits wide. The following table provides the names, corresponding numbers, and roles of each register:

| Register Number | Register Name | Purpose                     |
| --------------- | ------------- | --------------------------- |
| 0               | $zero         | Constant zero               |
| 1               | $imm          | Sign-extended immediate     |
| 2               | $v0           | Result value                |
| 3               | $a0           | Argument register           |
| 4               | $a1           | Argument register           |
| 5               | $t0           | Temporary register          |
| 6               | $t1           | Temporary register          |
| 7               | $t2           | Temporary register          |
| 8               | $t3           | Temporary register          |
| 9               | $s0           | Saved register              |
| 10              | $s1           | Saved register              |
| 11              | $s2           | Saved register              |
| 12              | $gp           | Global pointer (static data)|
| 13              | $sp           | Stack pointer               |
| 14              | $fp           | Frame Pointer               |
| 15              | $ra           | Return address              |


The registers names and their roles closely resemble those of the MIPS processor, with one notable difference: register number 1, $imm, is a special register that cannot be written to directly. It always contains the field of the immediate value, after undergoing sign extension. This register is updated for each instruction as part of the decoding process. Register 0, $zero, retains a constant zero value. Instructions that attempt to write to $zero do not alter its value.

## main memory 
Unlike the MIPS processor, the SIMP processor lacks support for byte or short data types. The word width is fixed at 32 bits, reflecting the width of the main memory. All operations involving main memory—whether reading or writing—are carried out with 32-bit units. Consequently, main memory addresses are expressed in terms of words, in contrast to the byte-oriented addresses used in MIPS. As a result, the program counter (PC) register advances normally (if not jumped) by 1, not by 4 as in the MIPS architecture.

## Instruction Set
The SIMP processor utilizes a uniform format for encoding all instructions. Each instruction is 32 bits wide and is encoded as follows: 

| 31:24|23:20|19:16|15:12|11:0|
|------|-----|-----|-----|----|
|opcode|rd   |rs   | rt |immediate|

The opcodes supported by the processor and the meaning of each instruction are given in the following table:

| Opcode Number | Name | Meaning |
|--------|------|---------|
| 0      | add  | R[rd]=R[rs]+R[rt] |
| 1      | sub  | R[rd]=R[rs]-R[rt] |
| 2      | and  | R[rd]=R[rs]&R[rt] |
| 3      | or   | R[rd]=R[rs]|R[rt] |
| 4      | sll  | R[rd]=R[rs]<<R[rt] |
| 5      | sra  | R[rd]=R[rs]+R[rt] |
| 6      | srl  | R[rd]=R[rs]>>>R[rt] |
| 7      | beq  | if (R[rs] == R[rt]) pc = R[rd] |
| 8      | bne  | if (R[rs] != R[rt]) pc = R[rd] |
| 9      | blt  | if (R[rs] < R[rt]) pc = R[rd] |
| 10     | bgt  | if (R[rs] > R[rt]) pc = R[rd] |
| 11     | ble  | if (R[rs] <= R[rt]) pc = R[rd] |
| 12     | bge  | if (R[rs] >= R[rt]) pc = R[rd] |
| 13     | jal  | R[15] = pc + 1 (next instruction address), pc = R[rd] |
| 14     | lw   | R[rd] = MEM[R[rs]+R[rt]] |
| 15     | sw   | MEM[R[rs]+R[rt]] = R[rd] |
| 16     | halt | Halt execution, exit simulator |

## The Simulator

The simulator operates on a fetch-decode-execute loop. Initially, PC is set to 0. In each iteration, the simulator fetches the instruction from the address held in PC, decodes the instruction, updates register number 1 with the sign-extended value of the immediate field, and then executes the instruction. After executing the instruction, PC is updated to PC + 1, unless a jump instruction has altered the PC to a different value. The simulation end after the execution of the HALT instruction.
The simulator was implemented in C and compiled into a command-line application. It accepts five command-line parameters as specified in the following execution line:

**sim.exe memin.txt memout.txt regout.txt trace.txt cycles.txt**

* **memin.txt File:**
  The `memin.txt` file is an input text file containing the initial contents of the main memory at the start of the simulation. Each line in the file represents a single memory cell (32 bits) starting at address zero, formatted as an 8-digit hexadecimal. If the number of lines in the file is less than 512 (the length of the main memory), it is assumed that the remaining memory cells are reset.

* **memout.txt File:**
  The `memout.txt` file is an output file, in the same format as `memin.txt`, which contains the contents of the main memory at the end of the run.

* **regout.txt File:**
  The `regout.txt` file is an output file that holds the contents of registers R2-R15 at the end of the simulation. Each line is formatted with 8 hexadecimal digits.

* **trace.txt File:**
  The `trace.txt` file is an output file containing a line of text for each instruction executed by the processor in the following format: PC INST R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15. Each field consists of 8 hexadecimal digits. The PC represents the Program Counter, INST is the encoded instruction read from memory, followed by the contents of the registers before executing the instruction.

* **cycles.txt File:**
  The `cycles.txt` file is an output file that records the number of clock cycles executed by the processor.








