;; Bootsect.asm
;; Created by Matheus Leme Da Silva
ORG 0x7C00
SECTION .text
BITS 16

START: EQU 1
COUNT: EQU 59
ADDR: EQU 0x500

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

	MOV [SectorsPerTrack], CL
	MOV [Heads], DH

	MOV SI, StartMessage
	CALL PrintString

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
	MOV AH, 0x0E
	MOV AL, 'O'
	INT 0x10
	MOV AL, 'K'
	INT 0x10

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
	MOV AL, AH
	CALL PrintHexNibble
	;; Low nibble
	CALL PrintHexNibble
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
	MOV BX, [SectorsPerTrack]
	DIV BX
	;; AX = LBA / SectorsPerTrack
	;; DX = LBA % SectorsPerTrack
	INC DL ;; Sector + 1
	MOV CL, DL

	XOR DX, DX
	XOR BX, BX
	MOV BL, [Heads]
	DIV BX
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
StartMessage: DB 'Loading', 0x00
ErrorMessage: DB 0x0D, 0x0A, 0x07, '!! Load error !!', 0x0D, 0x0A, 0x00
ErrorCodeMessage: DB 'Error Code: 0x', 0x00

Heads: DB 2
SectorsPerTrack: DB 18

TIMES 510 - ($ - $$) DB 0
DW 0x55AA
