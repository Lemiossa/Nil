/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */

typedef unsigned char Uint8;
typedef char Int8;
typedef unsigned short Uint16;
typedef short Int16;
typedef unsigned long Uint32;
typedef long Int32;

struct Regs {
	Uint16 ax;
	Uint16 bx;
	Uint16 cx;
	Uint16 dx;
	Uint16 di;
	Uint16 si;
	Uint16 es;
} __attribute__((packed));

void intx(Uint8 c, struct Regs *r);

// Print a char
void Putc(char c) {
	struct Regs r;
	r.ax = 0x0E00 | (c);
	intx(0x10, &r);
}

// Print a string
void Puts(char *s) {
	while (*s)
		Putc(*s++);
}

char *hexchars = "0123456789ABCDEF";

// Print Hex byte in little-endian
void PutHexByte(Uint8 b) {
	Putc((b >> 4) & 0x0F);
	Putc(b & 0x0F);
}

// Print Hex word in little-endian
void PutHexWord(Uint16 w) {
	PutHexByte((w >> 8) & 0xFF);
	PutHexByte(w & 0xFF);
}

// Print Hex dword in little-endian
void PutHexDword(Uint32 d) {
	PutHexWord((d >> 16) & 0xFFFF);
	PutHexWord(d & 0xFFFF);
}

void Main(void) {
	Puts("Hello World");
	while (1);
}
