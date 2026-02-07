; Bootsect.asm
; Created by Matheus Leme Da Silva
ORG 0x7C00
SECTION .text
BITS 16

JMP SHORT _start
NOP
TIMES 62 - ($ - $$) DB 0 ; BPB, Created by MKFS.FAT

GLOBAL _start
_start:
	CLI
	XOR AX, AX
	MOV DS, AX
	MOV ES, AX
	MOV SS, AX
	MOV SP, 0x7C00
	STI

	MOV [Drive], DL

	MOV SI, StartMessage
	CALL PrintString

Hang:
	CLI
	HLT

; Prints a string in DS:SI
PrintString:
	PUSH AX
	PUSH SI
	MOV AH, 0x0E
.Loop:
	LODSB
	CMP AL, 0
	JE .End
	INT 0x10
	JMP .Loop
.End:
	POP SI
	POP AX
	RET

SECTION .data
Drive: DB 0
StartMessage: DB 'Hello World', 0x0D, 0x0A, 0x00

TIMES 512 - ($ - $$) DB 0
DW 0x55AA
