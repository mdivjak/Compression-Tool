#include "menuHeader.h"
#include <time.h>

void callCompression(struct FilePaths fp, int log, int algorithm) {
	FILE *tar = NULL, *logFile = NULL;
	tar = fopen("tarball.bin", "wb");

	// open log file if necessary
	if (log) {
		// logName will be same as archive name with .log extension
		char logName[_MAX_DIR + 1];
		strcpy(logName, fp.archive);
		for (int j = strlen(logName) - 1; j >= 0; j--) {
			if (logName[j] == '.') {
				logName[j] = '\0';
				break;
			}
		}

		strcat(logName, ".log");
		logFile = fopen(logName, "w");
	}

	// merge all files
	make_tarball(fp.paths, fp.size, tar);
	fclose(tar);

	// call algorithm for compression
	FILE *in = fopen("tarball.bin", "rb");
	FILE *out = fopen(fp.archive, "wb");
	clock_t beginTime = clock();
	char byte = 0;
	switch (algorithm) {
	case DHUFF:	DH_compress(in, out); break;
	case SHUFF: SH_basic_compression(in, out); break;
	case LZW: lzw_basic_compression(in, out); break;
	case DEFLATE: DFL_compress(in, out); break;
	default: break;
	}

	fclose(in);
	fclose(out);
	clock_t endTime = clock();
	double executionTime = (double)(endTime - beginTime) / CLOCKS_PER_SEC;


	int beginSize, endSize;
	in = fopen("tarball.bin", "rb");
	out = fopen(fp.archive, "rb");
	beginSize = filesize(in);
	endSize = filesize(out);
	fclose(in); fclose(out);
	remove("tarball.bin");

	if (logFile) {
		fprintf(logFile, "Tool for Compression and Decompression - LOG\n"
			"\nTotal compression time: \t%.3f s\n"
			"\nSaved space: \t%.2f %%\n", executionTime, (double)(beginSize - endSize) / beginSize * 100);
		switch (algorithm) {
		case DHUFF: fprintf(logFile, "\nAlgorithm Used: Dynamic Huffman\n"); break;
		case SHUFF: fprintf(logFile, "\nAlgorithm Used: Static Huffman\n"); break;
		case LZW: fprintf(logFile, "\nAlgorithm Used: LZW\n"); break;
		case DEFLATE: fprintf(logFile, "\nAlgorithm Used: DEFLATE\n"); break;
		default: fprintf(logFile, "\nFatal Error...\n"); break;
		}
		fprintf(logFile, "\nFiles that were compressed:\n\n");
		for (int i = 0; i < fp.size; i++)
			fprintf(logFile, "%s\n", fp.paths[i]);

	}

	// close all files if necessary
	if (logFile) fclose(logFile);
	if (tar) fclose(tar);
}