/**
 * Bios.h
 * Created by Matheus Leme Da Silva
 */
#ifndef BIOS_H
#define BIOS_H
#include "Types.h"

union Regs {
	struct {
		Uint16 ax;
		Uint16 bx;
		Uint16 cx;
		Uint16 dx;
		Uint16 bp;
		Uint16 di;
		Uint16 si;
		Uint16 es;
		Uint16 flags;
	} w;

	struct {
		Uint8 al, ah;
		Uint8 bl, bh;
		Uint8 cl, ch;
		Uint8 dl, dh;
	} b;
};

#define FLAG_CF 0x0001
#define FLAG_PF 0x0004
#define FLAG_AF 0x0010
#define FLAG_ZF 0x0040
#define FLAG_SF 0x0080
#define FLAG_TF 0x0100
#define FLAG_IF 0x0200
#define FLAG_DF 0x0400
#define FLAG_OF 0x0800

void Intx(Uint8 c, union Regs *r);

void BIOSPutc(char c);
void BIOSSetVideoMode(Uint8 mode);
void BIOSDiskReset(Uint8 drive);
void BIOSDiskGetParameters(Uint8 drive, Uint8 *hds, Uint8 *spt);
char BIOSGetKeyboardKey(void);
char BIOSKeyboardCheck(void);
Uint8 BIOSDiskRead(Uint8 drive, Uint16 c, Uint8 h, Uint8 s, Uint8 n, Uint16 seg, Uint16 off);

#endif // BIOS_H
