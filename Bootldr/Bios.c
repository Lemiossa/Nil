/**
 * Bios.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Bios.h"
#include "Util.h"

// Print a char using BIOS
void BIOSPutc(char c)
{
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0x0E;
	r.b.al = c;
	Intx(0x10, &r);
	r.b.ah = 0x01;
	r.b.al = c;
	r.w.dx = 0;
	Intx(0x14, &r);
}

// Sets video mode using BIOS
void BIOSSetVideoMode(Uint8 mode)
{
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0;
	r.b.al = mode;
	Intx(0x10, &r);
}

// Reset drive using BIOS
void BIOSDiskReset(Uint8 drive)
{
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0x00;
	r.b.dl = drive;
	Intx(0x13, &r);
}

// Get keyboard key(ascii) using BIOS
char BIOSGetKeyboardKey(void) {
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0x00;
	Intx(0x16, &r);
	return r.b.al;
}

// Check if have key using BIOS
char BIOSKeyboardCheck(void) {
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0x01;
	Intx(0x16, &r);
	return !(r.w.flags & FLAG_ZF); // ZF if no keystroke
}

// Get disk parameters using BIOS
void BIOSDiskGetParameters(Uint8 drive, Uint8 *hds, Uint8 *spt)
{
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0x08;
	r.b.dl = drive;
	Intx(0x13, &r);
	if (hds)
		*hds = r.b.dh + 1;
	if (spt)
		*spt = r.b.cl & 0x3F;
}

// Read N sectors from disk using BIOS
Uint8 BIOSDiskRead(Uint8 drive, Uint16 c, Uint8 h, Uint8 s, Uint8 n, Uint16 seg, Uint16 off)
{
	union Regs r;
	Memset(&r, 0, sizeof(union Regs));
	r.b.ah = 0x02;
	r.b.al = n;
	r.b.ch = c & 0xFF;
	r.b.cl = (s & 0x3F) | ((c >> 2) & 0xC0);
	r.b.dh = h;
	r.b.dl = drive;
	r.w.es = seg;
	r.w.bx = off;
	Intx(0x13, &r);
	return r.b.ah;
}
