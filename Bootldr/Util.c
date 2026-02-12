/**
 * Util.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Util.h"
#include "Bios.h"
#include "Memdefs.h"

char *hexchars = "0123456789ABCDEF";

// Copy N bytes from S to D
void Memcpy(void *d, void *s, Uint16 n)
{
	Uint16 i = 0;
	Uint8 *dest = (Uint8 *)d;
	Uint8 *src = (Uint8 *)s;

	for (i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

// Set N bytes in D with B value
void Memset(void *d, Uint8 b, Uint16 n) {
	Uint16 i = 0;
	Uint8 *dest = (Uint8 *)d;
	for (i = 0; i < n; i++) {
		dest[i] = b;
	}
}

// Verify if s1 and s2 in memory is equal
int Memcmp(void *s1, void *s2, Uint16 n)
{
	Uint8 *src1 = (Uint8 *)s1;
	Uint8 *src2 = (Uint8 *)s2;
	Uint16 i = 0;
	if (!src1 || !src2)
		return 1;

	for (i = 0; i < n; i++) {
		if (src1[i] != src2[i])
			return src1[i] - src2[i];
	}

	return 0;
}

// Converts char to UPPERCASE
char ToUpper(char c)
{
	if (c >= 'a' && c <= 'z')
		return c - ('a' - 'A');
	return c;
}

// Converts char to lowercase
char ToLower(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c + ('a' - 'A');
	return c;
}

// Print a string
void Puts(char *s)
{
	while (*s)
		BIOSPutc(*s++);
}

// Print formated string
int PrintF(char *fmt, ...)
{
	int count = 0;
	char *digits = "0123456789abcdef";
	VaList args;
	if (!fmt)
		return 0;

	VaStart(args, fmt);

	while (*fmt) {
		if (*fmt == '%') {
			int base = 0;
			int size = 0;
			Uint16 num = 0;
			int neg = 0;
			char buf[32];
			int bufIdx = 0;
			int bufLen = 0;
			int upper = 0;
			int pad = 0;
			int negPad = 0;
			int zeroPad = 0;
			fmt++;

			if (*fmt == '-') {
				fmt++;
				negPad = 1;
			}

			if (*fmt == '0') {
				fmt++;
				zeroPad = 1;
			}

			while (*fmt >= '0' && *fmt <= '9') {
				pad = pad * 10 + (*fmt - '0');
				fmt++;
			}

			if (*fmt == 'h') {
				size = 2;
				fmt++;
				if (*fmt == 'h') {
					size = 1;
					fmt++;
				}
			}

			if (*fmt == 'd') {
				num = VaArg(args, int);
				base = 10;
				if (((Int16)num) < 0) {
					num = -num;
					neg = 1;
				}
			} else if (*fmt == 'u') {
				num = VaArg(args, Uint16);
				base = 10;
			} else if (*fmt == 'o') {
				num = VaArg(args, Uint16);
				base = 8;
			} else if (*fmt == 'x') {
				num = VaArg(args, Uint16);
				base = 16;
			} else if (*fmt == 'X') {
				num = VaArg(args, Uint16);
				base = 16;
				upper = 1;
			} else if (*fmt == 'b') {
				num = VaArg(args, Uint16);
				base = 2;
			} else if (*fmt == 'c') {
				BIOSPutc((char)VaArg(args, char));
				fmt++;
				count++;
				continue;
			} else if (*fmt == 's') {
				char *str = (char *)VaArg(args, char*);
				int strLen = 0;
				fmt++;

				if (!negPad) {
					int i = 0;
					for (i = 0; i < pad; i++) {
						BIOSPutc(' ');
						count++;
					}
				}

				if (!str) {
					Puts("(null)");
					count += 6;
					strLen = 6;
				} else {
					while (*str) {
						BIOSPutc(*str++);
						count++;
						strLen++;
					}
				}

				if (negPad) {
					int i = 0;
					for (i = 0; i < pad - strLen; i++) {
						BIOSPutc(' ');
						count++;
					}
				}
				continue;
			}
			fmt++;

			if (size == 1)
				num &= 0xFF;
			else if (size == 2)
				num &= 0xFFFF;

			if (num == 0) {
				buf[bufIdx++] = '0';
			} else {
				while (num > 0) {
					if (upper)
						buf[bufIdx++] = ToUpper(digits[num % base]);
					else
						buf[bufIdx++] = digits[num % base];

					num /= base;
				}
			}
			bufLen = bufIdx;

			if (neg) {
				BIOSPutc('-');
				count++;
			}

			if (!negPad) {
				int i = 0;
				char c = zeroPad?'0':' ';
				for (i = 0; i < pad - bufLen; i++) {
					BIOSPutc(c);
					count++;
				}
			}

			while (bufIdx-- > 0) {
				BIOSPutc(buf[bufIdx]);
				count++;
			}

			if (negPad) {
				int i = 0;
				for (i = 0; i < pad - bufLen; i++) {
					BIOSPutc(' ');
					count++;
				}
			}
		} else {
			BIOSPutc(*fmt++);
			count++;
		}
	}

	VaEnd(args);
	return count;
}

// Put pixel in graphics mode 320x200
void PutPixel(Uint16 x, Uint16 y, Uint8 c)
{
	Uint16 pos = y * 320 + x;
	FWriteByte(0xA000, pos, c);
}

// Get pixel in graphics mode 320x200
Uint8 GetPixel(Uint16 x, Uint16 y)
{
	Uint16 pos = y * 320 + x;
	return FReadByte(0xA000, pos);
}

// Read n sectors in a disk
// Max sector 65535
Uint8 DiskRead(void *d, Uint16 lba, Uint8 n, Uint8 drive)
{
	Uint8 h = 0;
	Uint8 s = 1;

	Uint8 hds = 2;
	Uint8 spt = 18;

	Uint8 ret = 0;

	int i = 0;
	Uint16 c = 0;

	BIOSDiskGetParameters(drive, &hds, &spt);
	if (hds == 0 || spt == 0)
		return 0x11;

	s = (lba % spt) + 1;
	h = (lba / spt) % hds;
	c = (lba / spt) / hds;

	for (i = 0; i < 3; i++) {
		ret = BIOSDiskRead(drive, c, h, s, n, GetCurrentDS(), (Uint16)d);

		if (ret == 0)
			return 0;

		BIOSDiskReset(drive);
	}

	return ret;
}

// Get string from keyboard input using BIOS
void Gets(char *out, int max) {
	int pos = 0;
	if (!out)
		return;

	while (1) {
		char c;
		while (!BIOSKeyboardCheck());
		c = BIOSGetKeyboardKey();

		if (c == '\r') {
			PrintF("\r\n");
			out[pos] = 0;
			return;
		} else if (c == '\b' && pos > 0) {
			PrintF("\b \b");
			pos--;
		} else if (c >= 32 && c <= 126) {
			BIOSPutc(c);
			out[pos++] = c;
		}
	}
}

// Copy path part(starting of 0) to OUT
// Return non zero if have error
int PathGetPart(char *path, int part, char *out, int maxOut)
{
	int curPart = 0;
	if (!path)
		return 1;

	if (*path == '/')
		path++;

	while (*path && curPart != part) {
		if (*path == '/') {
			path++;
			curPart++;
			continue;
		}

		path++;
	}

	if (curPart == part) {
		int outIdx = 0;
		while (*path != '/' && outIdx < maxOut) {
			out[outIdx++] = *path++;
		}
	} else {
		return 1;
	}

	return 0;
}
