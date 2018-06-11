#include "menuHeader.h"
#include <time.h>

void callDecompression(struct FilePaths fp, int log) {
	FILE *arch = NULL, *logFile = NULL;

	for (int i = 0; i < fp.size; i++) {
		char name[_MAX_DIR + 1], tarName[_MAX_DIR + 1];
		
		strcpy(name, fp.paths[i]);
		// delete archive extension
		for (int j = strlen(name) - 1; j >= 0; j--) {
			if (name[j] == '.') {
				name[j] = '\0';
				break;
			}
		}
		mkdir(name);

		//make log file if necessary
		if (log) {
			// logName will be same as archive name with .log extension
			char logName[_MAX_DIR + 1];
			strcpy(logName, name);
			strcat(logName, "\\Log.txt");

			logFile = fopen(logName, "w");
		}

		strcpy(tarName, name);
		strcat(tarName, "\\tar.bin");
		FILE *tarball = fopen(tarName, "wb");
		clock_t begin = clock();

		char algorithm;
		arch = fopen(fp.paths[i], "rb");
		fread(&algorithm, 1, 1, arch);
		
		switch (algorithm) {
		case DHUFF: DH_decompress(arch, tarball); break;
		case SHUFF:
			rewind(arch);
			SH_basic_decompression(arch, tarball); break;
		case LZW: lzw_basic_decompression(arch, tarball); break;
		case DEFLATE: DFL_decompress(arch, tarball); break;
		default:break;
		}

		clock_t end = clock();
		fclose(tarball);
		tarball = fopen(tarName, "rb");
		extract_tar(tarball, name);
		
		fclose(tarball);
		fclose(arch);
		int beginSize, endSize;
		arch = fopen(fp.paths[i], "rb"); tarball = fopen(tarName, "rb");
		beginSize = filesize(arch);
		endSize = filesize(tarball);
		fclose(arch); fclose(tarball);
		remove("tar.bin");
		if (log) {
			fprintf(logFile, "Tool for Compression and Decompression - LOG\n"
				"\nTotal compression time: \t%.3f s\n"
				"\nIncrease in space: \t%.2f %%\n", (double)(end - begin) / CLOCKS_PER_SEC, ((double)(endSize - beginSize) / beginSize) * 100);
			fclose(logFile);
		}
		remove(tarName);
	}
}