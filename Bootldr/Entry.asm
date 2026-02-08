;; Entry.asm
;; Created by Matheus Leme Da Silva
BITS 16
SECTION .text
EXTERN _Main

GLOBAL _start
_start:
	CLI
	XOR AX, AX
	MOV DS, AX
	MOV ES, AX
	MOV SS, AX
	MOV SP, 0xFFFF
	STI

	MOV [Drive], DL

	CALL _Main

	CLI
	HLT

;; Call BIOS with regs
;; struct Regs {
;;    unsigned short ax;
;;    unsigned short bx;
;;    unsigned short cx;
;;    unsigned short dx;
;;    unsigned short di;
;;    unsigned short si;
;;    unsigned short es;
;; };
;; void intx(unsigned char c, struct Regs *r);
GLOBAL _intx
_intx:
	PUSH BP
	MOV BP, SP
	PUSH AX
	MOV AL, [BP+4] ;; unsigned char c
	MOV [intx+1], AL

	PUSH DI
	PUSH SI
	PUSH ES
	PUSH DS
	PUSHA

	MOV SI, [BP+6]
	;; Struct
	;; SI+0 = AX
	;; SI+2 = BX
	;; SI+4 = CX
	;; SI+6 = DX
	;; SI+8 = DI
	;; SI+10 = SI
	;; SI+12 = ES

	MOV AX, [SI+0]
	MOV BX, [SI+2]
	MOV CX, [SI+4]
	MOV DX, [SI+6]
	MOV DI, [SI+8]
	PUSH WORD [SI+12]
	PUSH WORD [SI+10]
	POP SI
	POP ES

	PUSH BP
intx: INT 0x00
	POP BP
	PUSH SI

	MOV SI, [BP+6]
	MOV [SI+0], AX
	MOV [SI+2], BX
	MOV [SI+4], CX
	MOV [SI+6], DX
	MOV [SI+8], DI
	POP WORD [SI+10]
	MOV [DI+12], ES

	POPA

	POP DS
	POP ES
	POP SI
	POP DI

	POP AX
	POP BP
	RET

SECTION .data
Drive: DB 0
