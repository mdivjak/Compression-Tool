#include <stdio.h>
#include <stdlib.h>
#define BLOCK_SIZE 512
#define INT_BYTE 4
#define CHAR_BYTE 1

//returns number of bytes in a file provided pointer is at start
//it resets pointer back to start
int filesize(FILE *fp)
{
	int size;
	fseek(fp, 0, SEEK_END); // seek to end of file
	size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET); // seek back to beginning of file

	return size;
}

//makes single file and writes it into file pointer to by *tarball
//n - number of files
//filename[] - file paths of files
void make_tarball(char **filename, int n, FILE *tarball)
{
	FILE *fp;
	int size;
	int num_of_blocks;
	int trailing_bytes;
	int read;
	char buffer[BLOCK_SIZE];

	int i;
	for (i = 0; i < n; i++)
	{
		if ((fp = fopen(filename[i], "rb")) == NULL)
			continue;

		size = filesize(fp);
		num_of_blocks = (size / BLOCK_SIZE) + (size % BLOCK_SIZE != 0);
		trailing_bytes = num_of_blocks * BLOCK_SIZE - size;

		memcpy(buffer, &num_of_blocks, INT_BYTE);
		memcpy(buffer + INT_BYTE, &trailing_bytes, INT_BYTE);
		strcpy(buffer + 2 * INT_BYTE, filename[i]);
		fwrite(buffer, CHAR_BYTE, BLOCK_SIZE, tarball);

		while ((read = fread(buffer, CHAR_BYTE, BLOCK_SIZE, fp)) > 0)
		{
			fwrite(buffer, CHAR_BYTE, read, tarball);
			if (read < BLOCK_SIZE)
			{
				memset(buffer, 0, sizeof(buffer));
				fwrite(buffer, CHAR_BYTE, BLOCK_SIZE - read, tarball);
			}
		}

		fclose(fp);
	}

	memset(buffer, 0, sizeof(buffer));
	fwrite(buffer, CHAR_BYTE, BLOCK_SIZE, tarball);
}

//take whole filepath and returns filename
char *name_of_file(char *filepath)
{
	char *name = filepath + strlen(filepath) - 1;
	while (*name != '\\')
		name--;
	name++;
	return name;
}

//takes tarball and extracts all its file
//in location *location
//files are exactly the same as when compressed
//they also have same names and same extension
void extract_tar(FILE *in, char *location)
{
	FILE *fp;
	char buffer[BLOCK_SIZE];
	int num_of_blocks;
	int trailing_bytes;
	char filepath[1000];

	while (1)
	{
		fread(buffer, CHAR_BYTE, BLOCK_SIZE, in);

		memcpy(&num_of_blocks, buffer, INT_BYTE);
		memcpy(&trailing_bytes, buffer + INT_BYTE, INT_BYTE);

		strcpy(filepath, location);
		strcat(filepath, "\\");

		char *filename = name_of_file(buffer + 2 * INT_BYTE);
		if (strlen(filename) == 0) break;

		strcat(filepath, filename);

		if (num_of_blocks == 0)
			break;

		if ((fp = fopen(filepath, "wb")) == NULL)
			continue;

		int i;
		for (i = 0; i < num_of_blocks - 1; i++)
		{
			fread(buffer, CHAR_BYTE, BLOCK_SIZE, in);
			fwrite(buffer, CHAR_BYTE, BLOCK_SIZE, fp);
		}

		fread(buffer, CHAR_BYTE, BLOCK_SIZE, in);
		fwrite(buffer, CHAR_BYTE, BLOCK_SIZE - trailing_bytes, fp);

		fclose(fp);
	}
}