;; Bootsect.asm
;; Created by Matheus Leme Da Silva
ORG 0x7C00
SECTION .text
BITS 16

%define START 1
%define COUNT  59

%ifndef ADDR
%define ADDR 0x500
%endif

JMP SHORT _start
NOP
TIMES 62 - ($ - $$) DB 0 ;; BPB, Created by MKFS.FAT

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

	;; Get disk Geometry
	XOR DI, DI
	MOV AH, 0x08
	INT 0x13
	JC LoadError
	AND CL, 0x3F
	INC DH

	MOV BYTE [SectorsPerTrack], CL
	MOV BYTE [Heads], DH

	MOV SI, StartMessage
	CALL PrintString

	MOV AX, (ADDR & 0xFFFF)
	MOV DX, ((ADDR >> 16) & 0xFFFF)
	CALL PrintHexDword

	MOV AX, (ADDR >> 4)
	MOV ES, AX
	MOV BX, (ADDR & 0x0F)

	MOV AX, (START & 0xFFFF)
	MOV DX, ((START >> 16) & 0xFFFF)

	;; i = 0
	MOV CX, 0
LoadLoop:
	;; i++
	INC CX

	;; Increment segment if BX >= 0xFE00
	CMP BX, 0xFE00
	JB NoIncSegment
	SUB BX, 0xFE00

	PUSH AX
	MOV AX, ES
	ADD AX, 0xFE0
	MOV ES, AX
	POP AX
NoIncSegment:
	CALL ReadSector
	JC LoadError

	PUSH AX
	MOV AH, 0x0E
	MOV AL, '.'
	INT 0x10
	POP AX

	;; offset += 0x200
	ADD BX, 0x200

	;; lba++
	INC AX
	ADC DX, 0

	;; i < COUNT
	CMP CX, COUNT
	JB LoadLoop

	;; Loaded!
	MOV DL, [Drive]
	JMP (ADDR >> 4):(ADDR & 0x0F)

	JMP Halt

;; Prints error and halt the system
LoadError:
	MOV SI, ErrorMessage
	CALL PrintString

	MOV SI, ErrorCodeMessage
	CALL PrintString

	MOV AH, 0x01
	MOV DL, [Drive]
	INT 0x13
	MOV AL, AH
	CALL PrintHexByte

;; Halt the system
Halt:
	CLI
	HLT

;; Prints a string in DS:SI
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

;; Print a hex nibble in AL
PrintHexNibble:
	PUSH AX
	MOV AH, 0x0E
	AND AL, 0x0F
	CMP AL, 9
	JBE .digit
	ADD AL, 'A' - 10 ;; 0x40 - 0x46
	INT 0x10
	JMP .end
.digit:
	ADD AL, '0' ;; 0x30 - 0x39
	INT 0x10
.end:
	POP AX
	RET

;; Print a hex byte in AL
;; Little-endian
PrintHexByte:
	PUSH AX
	;; High nibble
	SHR AL, 4
	CALL PrintHexNibble
	POP AX
	;; Low nibble
	CALL PrintHexNibble
	RET

;; Print a hex word in AX
;; Little-endian
PrintHexWord:
	PUSH AX
	PUSH AX
	;; High Byte
	MOV AL, AH
	CALL PrintHexByte
	POP AX
	;; Low Byte
	CALL PrintHexByte
	POP AX
	RET

;; Print a hex dword in DX:AX
;; Little-endian
PrintHexDword:
	PUSH AX
	PUSH DX
	PUSH AX
	;; High Word
	MOV AX, DX
	CALL PrintHexWord
	POP AX
	;; Low Word
	CALL PrintHexWord
	POP DX
	POP AX
	RET

;; Convert LBA in DX:AX to CHS
;; AX = Cylinder
;; DH = Head
;; DL = Sector
LBAToCHS:
	PUSH BX
	PUSH CX

	;; Cylinder   = (LBA / SectorsPerTrack) / Heads
	;; Head       = (LBA / SectorsPerTrack) % Heads
	;; Sector     = (LBA % SectorsPerTrack) + 1
	DIV WORD [SectorsPerTrack]
	;; AX = LBA / SectorsPerTrack
	;; DX = LBA % SectorsPerTrack

	INC DL ;; Sector + 1
	MOV CL, DL

	XOR DX, DX
	DIV WORD [Heads]
	;; AX = (LBA / SectorsPerTrack) / Heads
	;; DX = (LBA / SectorsPerTrack) % Heads
	MOV DH, DL
	MOV DL, CL

	POP CX
	POP BX
	RET

;; Read a sector DX:AX in address ES:BX
ReadSector:
	PUSH DX
	PUSH AX
	PUSH CX
	CALL LBAToCHS

	;; CH = Cylinder Low
	;; CL = Sector | ((Cylinder High >> 2) & 0xC0)
	;; DH = Head
	MOV CH, AL
	MOV CL, AH
	SHR CL, 2
	AND CL, 0xC0
	OR CL, DL

	CLC
	MOV AX, 0x0201
	MOV DL, [Drive]
	INT 0x13

	POP CX
	POP AX
	POP DX
	RET

Drive: DB 0
StartMessage: DB 'Loading in 0x', 0x00
ErrorMessage: DB 0x0D, 0x0A, 0x07, '!! Load error !!', 0x0D, 0x0A, 0x00
ErrorCodeMessage: DB 'Error Code: 0x', 0x00

Heads: DW 2
SectorsPerTrack: DW 18

TIMES 510 - ($ - $$) DB 0
DW 0x55AA
