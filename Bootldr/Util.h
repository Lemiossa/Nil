/**
 * Util.h
 * Created by Matheus Leme Da Silva
 */
#ifndef UTIL_H
#define UTIL_H
#include "Types.h"

#define SECTOR_SIZE 512

Uint8 FReadByte(Uint16 seg, Uint16 off);
void FWriteByte(Uint16 seg, Uint16 off, Uint8 byte);
Uint16 FReadWord(Uint16 seg, Uint16 off);
void FWriteWord(Uint16 seg, Uint16 off, Uint16 word);

Uint16 GetCurrentDS(void);

void Memcpy(void *d, void *s, Uint16 n);
void Memset(void *d, Uint8 b, Uint16 n);
int Memcmp(void *s1, void *s2, Uint16 n);

char ToUpper(char c);
char ToLower(char c);

void Puts(char *s);
int PrintF(char *fmt, ...);
void PutPixel(Uint16 x, Uint16 y, Uint8 c);
Uint8 GetPixel(Uint16 x, Uint16 y);
Uint8 DiskRead(void *d, Uint16 lba, Uint8 n, Uint8 drive);
void Gets(char *out, int max);
int PathGetPart(char *path, int part, char *out, int maxOut);

#endif // UTIL_H

