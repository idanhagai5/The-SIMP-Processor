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
The simulator was implemented in C programming language and compiled into a command-line application. It accepts five command-line parameters as specified in the following execution line:

**sim.exe memin.txt memout.txt regout.txt trace.txt cycles.txt**

* **memin.txt File:**
The file `memin.txt` serves as an input file in text format, encapsulating the initial contents of the main memory at the beginning of program execution. Each line within the file represents the contents of a memory word, starting from address zero and presented in an 8-hexadecimal digit format. If the number of lines in the file is less than 512, it is implied that the remaining memory above the last specified address in the file is reset.

* **memout.txt File:**
  The `memout.txt` file is an output file, in the same format as `memin.txt`, which contains the contents of the main memory at the end of the run.

* **regout.txt File:**
  The `regout.txt` file is an output file that holds the contents of registers R2-R15 at the end of the simulation. Each line is formatted with 8 hexadecimal digits.

* **trace.txt File:**
  The `trace.txt` file is an output file containing a line of text for each instruction executed by the processor in the following format: PC INST R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10 R11 R12 R13 R14 R15. Each field consists of 8 hexadecimal digits. The PC represents the Program Counter, INST is the encoded instruction read from memory, followed by the contents of the registers before executing the instruction.

* **cycles.txt File:**
  The `cycles.txt` file is an output file that records the number of clock cycles executed by the processor.

## The Assembler

The assembler, developed in the C programming language, is responsible for translating programs written in assembly language into machine language. Similar to the simulator, the assembler operates as a command-line application with the following execution syntax:

**asm.exe program.asm mem.txt**

The input file `program.asm` stores the assembly program, while the output file `mem.txt` save the resulting memory image. The output file generated by the assembler serves as the input file for the simulator.

Each line of code in the assembly file contains all 5 parameters of the encoded instruction. The first parameter is the opcode, followed by the remaining parameters separated by commas. 

In each instruction, the immediate field can take one of the following forms:
- A decimal number, either positive or negative.
- A hexadecimal number, starting with '0x' and followed by hexadecimal digits.
- A label.

### Examples:

```assembly
bne $imm, $t0, $t1, L1       # if ($t0 != $t1) goto L1 (reg1 = address of L1)
add $t2, $t2, $imm, 1        # $t2 = $t2 + 1 (reg1 = 1)

beq $imm, $zero, $zero, L2   # unconditional jump to L2 (reg1 = address L2)
L1:
sub $t2, $t2, $imm, 1        # $t2 = $t2 – 1 (reg1 = 1)

L2:
add $t1, $zero, $imm, L3     # $t1 = address of L3 (reg1 = address L3)
beq $t1, $zero, $zero, 0      # jump to the address specified in t1 (reg1 = 0)

L3:
jal $imm, $zero, $zero, L4    # function call L4, save return addr in $ra
halt $zero, $zero, $zero, 0   # halt execution

L4:
beq $ra, $zero, $zero, 0       # return from function in the address in $ra (reg1 = 0)
```

The assembler performs a two-pass scan to support labels in the assembly code. During the first pass, it records the addresses of all the labels encountered. In the second pass, it replaces the labels in the immediate field with the corresponding addresses calculated during the first scan. In addition to processing assembly instructions, the assembler also supports a specific instruction that enables the direct placement of a 32-bit word into memory. This instruction facilitates the incorporation of data into the memory image file:

**.word address data**

In this syntax, `address` signifies the word address in memory, and `data` represents the content of the word. Both fields accept either decimal numbers or hexadecimal numbers starting with 0x

### Examples:
```assembly
.word 256 1 # set MEM[256] = 1
.word 0x100 0x1234ABCD # MEM[0x100] = MEM[256] = 0x1234ABCD
```

## Test Programs 

Three assembly programs were created to validate the correctness of the assembler and simulator:

1. **Bubble Sort Program (bubble.asm):**
   - Implements the bubble sort algorithm to sort an array of numbers in ascending order.
   - The array to be sorted is located in memory cells 0x100 to 0x10F.

2. **Matrix Addition Program (summat.asm):**
   - Performs the addition of two 4x4-valued matrices.
   - The first matrix is located at memory addresses 0x100 to 0x10F.
   - The second matrix is located at memory addresses 0x110 to 0x11F.
   - The result matrix will be written to memory addresses 0x120 to 0x12F.
   - For example, for the first matrix, A11 will be at address 0x100, A12 at address 0x101, and so on.


3. **Binomial Coefficient Program (binom.asm):**
   - Calculates Newton's binomial coefficient recursively using the following algorithm:
     ```c
     int binom(n, k) {
         if (k == 0 || n == k)
             return 1;
         return binom(n-1, k-1) + binom(n-1, k);
     }
     ```
   - At the start of the run, the values of `n` and `k` are provided at memory addresses 0x100 and 0x101, respectively.
   - The result will be written to memory address 0x102.
   - It is assumed that `n` is small enough to avoid overflow.

  
     4. **Fibonacci Program (fib.asm):**
   - Calculates the n'th term of a Fibonacci series.
   - The input 'n' is stored in memory at address 0x20..
   - The result will be written to memory address 0x21.
   - It is assumed that `n` is small enough to avoid overflow.











