#include "menuHeader.h"

void addPathsCMD(int argc, char *argv[], struct FilePaths *fp, int i, int archives) {
	for (; i < argc; i++) {
		char tmpName[_MAX_DIR + 1], pathName[_MAX_DIR + 1], fileName[_MAX_DIR + 1];
		// if argument is not wildcard then we have to add only one file path
		if (isPath(argv[i])) {
			// if argument is not full path add current directory and then file name
			if (!fullPath(argv[i])) {
				getcwd(fileName);
				strcat(fileName, "\\");
				strcat(fileName, argv[i]);
			}
			else strcpy(fileName, argv[i]);

			// add files that are readable
			if (isReadable(fileName)) {
				if (archives && isArchive(fileName))
					addFilePath(fp, fileName);
				else if (!archives)
					addFilePath(fp, fileName);
			}
		}
		// argument contains wildcards
		else {

			if (!fullPath(argv[i])) {
				getcwd(tmpName);
				strcat(tmpName, "\\");
				strcat(tmpName, argv[i]);
			}
			else strcpy(tmpName, argv[i]);
			strcpy(pathName, tmpName);
			
			for (int j = strlen(pathName) - 1; j >= 0; j--) {
				if (pathName[j] == '\\') {
					pathName[j] = '\0';
					break;
				}
			}

			// tmpName now contains full path to selected files
			// pathName now contains path to directory which contains the files
			DIR *dir = opendir(pathName);
			struct dirent *file;
			while (file = readdir(dir)) {
				strcpy(fileName, pathName);
				strcat(fileName, "\\");
				strcat(fileName, file->d_name);

				// if file matches pattern and is a readable file then add it to list
				if (isMatch(fileName, tmpName) && file->d_type == DT_REG && isReadable(fileName)) {
					if (archives && isArchive(fileName))
						addFilePath(fp, fileName);
					else if (!archives)
						addFilePath(fp, fileName);
				}
			}
			closedir(dir);
		}
	}
}

int isMatch(char *str, char *ptr) {
	int strSize = strlen(str);
	int ptrSize = strlen(ptr);

	int **result, x;
	result = (int**)calloc(strSize + 1, sizeof(int*));
	for (int i = 0; i < strSize + 1; i++)
		result[i] = (int*)calloc(ptrSize + 1, sizeof(int));

	result[0][0] = 1;

	for (int i = 1; i <= ptrSize; i++)
		result[0][i] = 0;

	for (int i = 1; i <= strSize; i++)
		result[i][0] = 0;

	for (int i = 1; i <= ptrSize; i++) {
		if (ptr[i - 1] == '*')
			result[0][i] = result[0][i - 1];
	}

	for (int i = 1; i <= strSize; i++) {
		for (int j = 1; j <= ptrSize; j++) {
			if (str[i - 1] == ptr[j - 1] || ptr[j - 1] == '?')
				result[i][j] = result[i - 1][j - 1];
			else if (ptr[j - 1] == '*')
				result[i][j] = result[i][j - 1] || result[i - 1][j];
			else
				result[i][j] = 0;
		}
	}

	x = result[strSize][ptrSize];
	for (int i = 0; i < strSize + 1; i++)
		free(result[i]);
	free(result);
	result = NULL;

	return x;
}

int fullPath(char *p) {
	for (int i = 0; i < strlen(p); i++) {
		if (p[i] == ':') return 1;
	}
	return 0;
}

int isPath(char *p) {
	int wildcard = 0;
	int i = strlen(p) - 1;

	// check every character from last until backslash
	// if it is wildcard character
	while (i >= 0 && p[i] != '\\') {
		if (p[i] == '*' || p[i] == '?') return 0;
		i--;
	}

	return 1;
}

void writeHelp(char *path) {
	int i = strlen(path) - 1;
	while (i >= 0 && path[i] != '\\') i--;
	i++;
	printf("USAGE:\n");
	printf("\t%s [-h | -? | -a]\n", path + i);
	printf("\t%s [-c] [-dhuff | -shuff | -lzw | -def] [-l] [paths] [arch]\n", path + i);
	printf("\t%s [-d] [-l] [paths]\n", path + i);

	printf("\nOPTIONS:\n"
		"\t-h \tDisplay this help message.\n"
		"\t-? \tDisplay this help message.\n"
		"\t-a \tDisplay information about the program.\n"
		"\t-c \tProgram will compress files.\n"
		"\t-dhuff \tDynamic Huffman algorithm will be used for compression.\n"
		"\t-shuff \tStatic Huffman algorithm will be used for compression.\n"
		"\t-lzw \tLZW algorithm will be used for compression.\n"
		"\t-def \tDEFLATE algorithm will be used for compression.\n"
		"\t-d \tProgram will decompress archives.\n"
		"\t-l \tProgram will maintain a log file tracking compression / decompression process.\n"
		"\tpaths \tPaths to specified files / archives to compress / decompress.\n"
		"\tarch \tName of the archive in which the files will be compressed.\n");

	printf("\nEXAMPLES:\n"
		"\t%s -h\n"
		"\t%s -c -lzw -l Notes.txt C:\\Users\\JohnDoe\\Documents\\*.docx myArchive.bin\n"
		"\t%s -d C:\\Users\\JohnDoe\\Desktop\\ZippedRe?orts.bin\n\n", path + i, path + i, path + i);
}

void writeAbout() {
	printf("\n\tThis program is a simple tool to compress files or "
		"decompress archives.\n\tThe program demonstrates compression using Dynamic Huffman "
		"\n\tand Static Huffman algorithm, LZW and DEFLATE algorithm for compression."
		"\n\n\tThe Text User Interface was created using the "
		"PDcurses library for graphical interface.\n\n\tThe entire project "
		"was built in C programming language.\n");

	printf("\n\tFaculty of Electrical Engineering - University of Belgrade 2018\n\t"
		"Students: Marko Divjak, Uros Isakovic, Dusan Vojinovic\n");
}

void writeError() {
	printf("\n\tThe syntax of the command is incorrect.\n");
	printf("\n\tUse -h or -? for help\n");
}
