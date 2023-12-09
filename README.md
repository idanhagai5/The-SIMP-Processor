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

