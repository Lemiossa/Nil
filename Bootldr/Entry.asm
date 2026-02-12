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
	MOV SP, StackTop

	MOV [_Drive], DL

	CALL _Main

	CLI
	HLT

;; Call BIOS with regs
;; struct Regs {
;;    unsigned short ax;
;;    unsigned short bx;
;;    unsigned short cx;
;;    unsigned short dx;
;;    unsigned short bp;
;;    unsigned short di;
;;    unsigned short si;
;;    unsigned short es;
;;    unsigned short flags;
;; };
;; NOTE: Flags is read-only
;; void Intx(unsigned char c, struct Regs *r);
GLOBAL _Intx
_Intx:
	PUSH BP
	MOV BP, SP

	MOV AL, [BP+4] ;; unsigned char c
	MOV [.Int+1], AL

	MOV [.BP], BP

	PUSH SI
	PUSH DI
	PUSH ES

	;; Struct
	;; SI+0 = AX
	;; SI+2 = BX
	;; SI+4 = CX
	;; SI+6 = DX
	;; SI+8 = BP
	;; SI+10 = DI
	;; SI+12 = SI
	;; SI+14 = ES
	;; SI+16 = FLAGS

	MOV SI, [BP+6]
	PUSH WORD [SI+0]  ;; AX
	PUSH WORD [SI+2]  ;; BX
	PUSH WORD [SI+4]  ;; CX
	PUSH WORD [SI+6]  ;; DX
	PUSH WORD [SI+8]  ;; BP
	PUSH WORD [SI+10] ;; DI
	PUSH WORD [SI+12] ;; SI
	PUSH WORD [SI+14] ;; ES

	POP ES
	POP SI
	POP DI
	POP BP
	POP DX
	POP CX
	POP BX
	POP AX

.Int:
	INT 0x00

	PUSH AX
	PUSH BX
	PUSH CX
	PUSH DX
	PUSH BP
	PUSH DI
	PUSH SI
	PUSH ES
	PUSHF

	MOV BP, [.BP]
	MOV SI, [BP+6]
	POP WORD [SI+16] ;; FLAGS
	POP WORD [SI+14] ;; ES
	POP WORD [SI+12] ;; SI
	POP WORD [SI+10] ;; DI
	POP WORD [SI+8]  ;; BP
	POP WORD [SI+6]  ;; DX
	POP WORD [SI+4]  ;; CX
	POP WORD [SI+2]  ;; BX
	POP WORD [SI+0]  ;; AX

	POP ES
	POP DI
	POP SI

	POP BP
	RET
.BP: DW 0

;; Returns current Data segment
;; Uint16 GetCurrentDS(void);
GLOBAL _GetCurrentDS
_GetCurrentDS:
	PUSH DS
	POP AX
	RET

;; Read far memory byte
;; Uint8 FReadByte(Uint16 seg, Uint16 off);
GLOBAL _FReadByte
_FReadByte:
	PUSH BP
	MOV BP, SP
	PUSH ES
	PUSH DI
	PUSH BX

	MOV AX, [BP+4]
	MOV ES, AX
	MOV DI, [BP+6]

	XOR AX, AX
	MOV AL, [ES:DI]

	POP BX
	POP DI
	POP ES
	POP BP
	RET

;; Write far memory byte
;; void FWriteByte(Uint16 seg, Uint16 off, Uint8 byte);
GLOBAL _FWriteByte
_FWriteByte:
	PUSH BP
	MOV BP, SP
	PUSH ES
	PUSH DI
	PUSH BX

	MOV AX, [BP+4]
	MOV ES, AX
	MOV DI, [BP+6]

	MOV AL, [BP+8]
	MOV [ES:DI], AL

	POP BX
	POP DI
	POP ES
	POP BP
	RET

;; Read far memory word
;; Uint16 FReadWord(Uint16 seg, Uint16 off);
GLOBAL _FReadWord
_FReadWord:
	PUSH BP
	MOV BP, SP
	PUSH ES
	PUSH DI
	PUSH BX

	MOV AX, [BP+4]
	MOV ES, AX
	MOV DI, [BP+6]

	MOV AX, [ES:DI]

	POP BX
	POP DI
	POP ES
	POP BP
	RET

;; Write far memory word
;; void FWriteWord(Uint16 seg, Uint16 off, Uint16 word);
GLOBAL _FWriteWord
_FWriteWord:
	PUSH BP
	MOV BP, SP
	PUSH ES
	PUSH DI
	PUSH BX

	MOV AX, [BP+4]
	MOV ES, AX
	MOV DI, [BP+6]

	MOV AX, [BP+8]
	MOV [ES:DI], AX

	POP BX
	POP DI
	POP ES
	POP BP
	RET

SECTION .data
GLOBAL _Drive
_Drive: DB 0

SECTION .bss
StackBottom:
	RESB 4096
StackTop:

