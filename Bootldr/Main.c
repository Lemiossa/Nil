/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Bios.h"
#include "Util.h"

extern Uint8 Drive;
struct FatBpb {
	Uint8 Jump[3];
	Uint8 OemName[8];
	Uint16 BytesPerSector;
	Uint8 SectorsPerCluster;
	Uint16 ReservedSectors;
	Uint8 NumFats;
	Uint16 RootDirEntries;
	Uint16 TotalSectors16;
	Uint8 MediaDesc;
	Uint16 SectorsPerFat;
	Uint16 SectorsPerTrack;
	Uint16 Heads;
	Uint16 HiddenSectorsLow;
	Uint16 HiddenSectorsHigh;
	Uint16 TotalSectors32Low;
	Uint16 TotalSectors32High;
};

struct FatEntry {
	Uint8 Name[11];
	Uint8 Attr;
	Uint8 Res;
	Uint8 CTimeInHund;
	Uint16 CTime; // Hour = 5 bits;
				  // Min = 6 bits;
				  // Sec = 5 bits *NOTE: multiply secs per 2*
	Uint16 CDate; // Year = 7 bits; *NOTE: Year + 1980*
				  // Month = 4 bits;
				  // Day = 5 bits
	Uint16 ADate; // Equal CDate
	Uint16 ClusterHigh;
	Uint16 MTime; // Equal CTime
	Uint16 MDate; // Equal CDate
	Uint16 ClusterLow;
	Uint16 FileSizeLow;
	Uint16 FileSizeHigh;
};

#define FAT_READ_ONLY 0x01
#define FAT_HIDDEN 0x02
#define FAT_SYSTEM 0x04
#define FAT_VOLUME_ID 0x08
#define FAT_DIRECTORY 0x10
#define FAT_ARCHIVE 0x20
// FUCK YOU, LFN!

struct FatBpb Bpb;


Uint8 GFatType = 0;
Uint16 GTotalSectors = 0;
Uint16 GRootDirSectors = 0;
Uint16 GDataSectors = 0;
Uint16 GTotalClusters = 0;

Uint16 GDataLBA = 0;
Uint16 GFatLBA = 0;
Uint16 GRootLBA = 0;

// Extract hour, min, sec from FAT format
void FATTimeToNormalTime(Uint16 FATTime, Uint8 *hour, Uint8 *min, Uint8 *sec) {
	if (hour)
		*hour = (FATTime & 0xF800) >> 11;

	if (min)
		*min = (FATTime & 0x7E0) >> 5;

	if (sec)
		*sec = (FATTime & 0x1F) * 2;
}

// Extract year, month, day from FAT format
void FATDateToNormalDate(Uint16 FATDate, Uint16 *year, Uint8 *month, Uint8 *day) {
	if (year)
		*year = ((FATDate & 0xFE00) >> 9) + 1980;

	if (month)
		*month = (FATDate & 0x1E0) >> 5;

	if (day)
		*day = FATDate & 0x1F;
}

// Converts FILENAME to FATNAME
void FATFilenameToFATName(char *filename, char *out)
{
	int i = 0, ext = 0;
	if (!filename || !out)
		return;

	for (i = 0; i < 11; i++) {
		out[i] = ' ';
	}

	for (i = 0; i < 8; i++) {
		char c = ToUpper(filename[i]);
		if (c == 0)
			break;

		if (c == '.') {
			ext = i;
			break;
		}
		out[i] = c;
	}

	if (ext == 0)
		return;

	for (i = 0; i < 3; i++) {
		char c = ToUpper(filename[i + ext + 1]);
		out[i + 8] = c;
	}
}

// Read fat NEXT cluster
Uint16 FATNextCluster(Uint16 cluster)
{
	Uint8 buf[SECTOR_SIZE * 2];
	Uint16 offset, sector, entryOff, val;

	if (GFatType == 12)
		offset = cluster + (cluster / 2);
	else
		offset = cluster * 2;

	sector = GFatLBA + (offset / SECTOR_SIZE);
	entryOff = offset % SECTOR_SIZE;
	val = 0;
	DiskRead(buf, sector, 1, Drive);
	DiskRead(&buf[SECTOR_SIZE], sector+1, 1, Drive);


	if (GFatType == 12) {
		Uint16 entryVal = *(Uint16 *)&buf[entryOff];

		if (cluster & 1)
			val = entryVal >> 4;
		else
			val = entryVal & 0x0FFF;

	} else if (GFatType == 16) {
		val = *(Uint16 *)&buf[entryOff];
	}

	return val;
}

// Return 1 if cluster is END
int FATClusterEnd(Uint16 cluster)
{
	Uint16 val;
	if (GFatType == 12)
		val = 0xFF8;
	else if (GFatType == 16)
		val = 0xFFF8;
	else
		return 1;
	return (cluster >= val);
}

// Return 1 if cluster is BAD
int FATClusterBad(Uint16 cluster)
{
	Uint16 val;
	if (GFatType == 12)
		val = 0xFF7;
	else if (GFatType == 16)
		val = 0xFFF7;
	else
		return 1;
	return (cluster == val);
}

// Converts cluster to LBA
Uint16 FATClusterToLBA(Uint16 cluster)
{
	return ((cluster - 2) * Bpb.SectorsPerCluster) + GDataLBA;
}

// Initializes FAT floppy system
int FATInit(void)
{
	Uint8 buf[512];
	if (DiskRead(buf, 0, 1, Drive) != 0) {
		Puts("FATInit(): Failed to read sector 0\r\n");
		return 1;
	}

	Bpb.BytesPerSector = *(Uint16*)&buf[0x0B];
	Bpb.SectorsPerCluster = buf[0x0D];
	Bpb.ReservedSectors = *(Uint16*)&buf[0x0E];
	Bpb.NumFats = buf[0x10];
	Bpb.RootDirEntries = *(Uint16*)&buf[0x11];
	Bpb.TotalSectors16 = *(Uint16*)&buf[0x13];
	Bpb.MediaDesc = buf[0x15];
	Bpb.SectorsPerFat = *(Uint16*)&buf[0x16];

	if (Bpb.SectorsPerFat == 0) {
		Puts("FATInit(): Invalid BPB\r\n");
		return 1;
	}

	if (Bpb.TotalSectors16 == 0)
		return 1;

	GTotalSectors = Bpb.TotalSectors16;
	GRootDirSectors = ((Bpb.RootDirEntries * 32) + (SECTOR_SIZE - 1)) / SECTOR_SIZE;

	GDataLBA = Bpb.ReservedSectors + (Bpb.NumFats * Bpb.SectorsPerFat) + GRootDirSectors;
	GFatLBA = Bpb.ReservedSectors;
	GRootLBA = GDataLBA - GRootDirSectors;

	GDataSectors = GTotalSectors - (Bpb.ReservedSectors + (Bpb.NumFats * Bpb.SectorsPerFat) + GRootDirSectors);

	GTotalClusters = GDataSectors / Bpb.SectorsPerCluster;

	if (GTotalClusters < 4085) {
		GFatType = 12;
	} else {
		GFatType = 16;
	}

	return 0;
}

// Read dir in offset
// If cluster == 0: Read root dir
// Return non zero if have error
int FATReadDir(Uint16 cluster, struct FatEntry *out, Uint16 index)
{
	Uint16 curCluster = cluster;
	Uint16 curIndex = 0;

	if (GFatType == 0)
		return 1;

	if (curCluster < 2 && !(GFatType == 12 || GFatType == 16))
		return 1;

	while (!FATClusterEnd(curCluster)) {
		int i = 0;
		Uint16 sectors = curCluster == 0 ? GRootDirSectors : Bpb.SectorsPerCluster;
		for (i = 0; i < sectors; i++) {
			Uint8 buf[SECTOR_SIZE];
			Uint16 j = 0;
			Uint16 lba = curCluster == 0 ? (GRootLBA + i) : (FATClusterToLBA(curCluster) + i);
			if (DiskRead(buf, lba, 1, Drive) != 0)
				return 2;

			for (j = 0; j < SECTOR_SIZE / 32; j++) {
				struct FatEntry entry;
				Uint8 *entryBuf = &buf[j * 32];
				Memcpy(entry.Name, &entryBuf[0], 11);
				entry.Attr = entryBuf[11];
				entry.Res = entryBuf[12];
				entry.CTimeInHund = entryBuf[13];
				entry.CTime = *(Uint16*)&entryBuf[14];
				entry.CDate = *(Uint16*)&entryBuf[16];
				entry.ADate = *(Uint16*)&entryBuf[18];
				entry.ClusterHigh = *(Uint16*)&entryBuf[20];
				entry.MTime = *(Uint16*)&entryBuf[22];
				entry.MDate = *(Uint16*)&entryBuf[24];
				entry.ClusterLow = *(Uint16*)&entryBuf[26];
				entry.FileSizeLow = *(Uint16*)&entryBuf[28];
				entry.FileSizeHigh = *(Uint16*)&entryBuf[30];

				if (entry.Name[0] == 0)
					goto end;

				if (entry.Name[0] == 0xE5)
					continue;

				if (curIndex == index) {
					if (out)
						Memcpy(out, &entry, 32);
					return 0;
				}

				curIndex++;
			}
		}

		if (curCluster == 0)
			break;

		curCluster = FATNextCluster(curCluster);
	}

end:
	return 1; // Not found
}

// Find entry in FAT dir
// If cluster == 0: Read root dir
// Return non zero if have error
int FATFindInDir(Uint16 cluster, char *filename, struct FatEntry *out)
{
	Uint16 index = 0;
	char fatName[11];

	if (!filename)
		return 1;

	Memset(fatName, 0, 11);

	FATFilenameToFATName(filename, fatName);

	while (1) {
		struct FatEntry entry;
		if (FATReadDir(cluster, &entry, index) != 0)
			break;

		if (Memcmp(entry.Name, fatName, 11) == 0) {
			if (out)
				Memcpy(out, &entry, 32);
			return 0;
		}

		index++;
	}

	return 1; // Not found
}

// Find entry in complete path
// Return non zero if have error
int FATFind(char *path, struct FatEntry *out) {
	char partStr[12];
	int curPart = 0;
	Uint16 curCluster = 0;
	if (!path)
		return 1;

	while (1) {
		struct FatEntry entry;
		Memset(partStr, 0, 12);
		if (PathGetPart(path, curPart++, partStr, 12) != 0)
			break;

		if (FATFindInDir(curCluster, partStr, &entry) != 0)
			break;

		if (entry.Attr & FAT_DIRECTORY) {
			curCluster = entry.ClusterLow;
			continue;
		}

		if (out)
			Memcpy(out, &entry, sizeof(struct FatEntry));
		return 0;
	}

	return 1; // Not found
}

// Main function
void Main(void)
{
	struct FatEntry entry;
	Puts("\r\nLoading...\r\n");
	if (FATInit())
		Puts("Failed to initialize FAT system\r\n");
	else
		Puts("Initilized FAT system!\r\n");

	if (FATFind("/anydir/text.txt", &entry) == 0) {
		PrintF("Found /anydir/text.txt!\r\n");
	}

	while (1);
}
