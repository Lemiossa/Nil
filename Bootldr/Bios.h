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
	} w;

	struct {
		Uint8 al, ah;
		Uint8 bl, bh;
		Uint8 cl, ch;
		Uint8 dl, dh;
	} b;
};

void Intx(Uint8 c, union Regs *r);

void BiosPutc(char c);
void BiosSetVideoMode(Uint8 mode);
void BiosDiskReset(Uint8 drive);
void BiosDiskGetParameters(Uint8 drive, Uint8 *hds, Uint8 *spt);
Uint8 BiosDiskRead(Uint8 drive, Uint16 c, Uint8 h, Uint8 s, Uint8 n, Uint16 seg, Uint16 off);


#endif // BIOS_H
