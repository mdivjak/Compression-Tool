#pragma once
#ifndef __MENU_HEADER
#define __MENU_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "curses.h"
#include "dirent.h"

#define FOOTER_HEIGHT 2
#define HEADER_HEIGHT 2
#define BODY_HEIGHT LINES-FOOTER_HEIGHT-HEADER_HEIGHT
// codes for enter and right enter keys
#define KEY_ENTER 10
#define KEY_RENTER 459
#define MENU_OPTIONS 3
#define PROMPT_HEIGHT 6
#define PROMPT_WIDTH 33
#define SELECT_HEIGHT 7
#define SELECT_WIDTH 23
#define INFO_HEIGHT 7
#define INFO_WIDTH 33

#define LISTING_INCREMENT 20
#define FILEPATH_INCREMENT 10

#define T_DIRECTORY 1
#define T_FILE 0
#define T_UNDEF -1

#define NO_COLOR 1

#define BLOCK_SIZE 512
#define INT_BYTE 4
#define CHAR_BYTE 1

//algorithm macros
#define DHUFF 0
#define SHUFF 1
#define LZW 2
#define DEFLATE 3

static char *choices[] = {
	"Compress",
	"Decompress",
	"Exit"
};

static char extension[] = ".bin";
static char title[] = "Compression and Decompression Tool";

struct Choice {
	char *name;
	int checked, type;
};

struct Listing {
	int capacity, size, start, end;
	struct Choice **ls;
};

struct FilePaths {
	int capacity, size;
	char **paths;
	char archive[_MAX_DIR + 1];
};

//FUNCTION PROTOTYPES
//main.c
/**
* @brief Function starts the graphical or Text User Interface.
* Function enables the user to choose some of the program options.
* After that file manager for selection files to compress will start
* and afterwards the paths to those files will be passed to compression and decompression functions.
*/
void startTUI();

/**
* @brief Function that is called when the command line interface is used.
* @param[in] argc Number of arguments passed through the command line.
* @param[in] argv Array of strings that were passed as arguments to the program.
*/
void startCMD(int argc, char *argv[]);

//tarball.c
/**
* @brief Function returns size of file in bytes.
* @param[in] fp Pointer to file whose size is to be returned.
* return int Size of file in bytes.
*/
int filesize(FILE *fp);

/**
* @brief Functions takes an array of string that contain paths to files
* and merges all of them into one tarball.
* @param[in] filename Array of strings that contain paths to files.
* @param[in] n Number of strings in the array.
* @param[in] tarball Pointer to file to write all files.
*/
void make_tarball(char **filename, int n, FILE *tarball);

/**
* @brief Function returns pointer to the first letter of filename inside string filepath.
* @param[in] filepath String that contains full path to file.
* @return char* Pointer to first letter of the name of the file.
*/
char* name_of_file(char *filepath);

/**
* @brief Function unmerges files merged with make_tarball(char**, int, FILE*);
* @param[in] in Pointer to file of all merged files.
* @param[in] location String that contains path to folder in which the files will be extracted to.
*/
void extract_tar(FILE *in, char *location);


//menuFunctions.c
/**
* @brief Function write message to a info window.
* @param[in] info Pointer to window in which the message will be written.
* @param[in] messg Message to be written in the window.
*/
void infoMessage(WINDOW *info, char *messg);

/**
* @brief Function write window in which the user chooses which
* algorithms will be used for compression.
* @return int Returns index of the algorithm that will be used for compression.
*/
int selectAlgorithm();

/**
* @brief Function writes algorithms that can be chosen into the window algo.
* and marks the choice selected with index highlight.
* @param[in] algo Pointer to window in which the list with algorithms will be written.
* @param[in] highlight Index of the option currently selected.
*/
void promptAlgorithms(WINDOW *algo, int highlight);

/**
* @brief Function writes the header of the program. The header contains the program title
* and options that the user can choose.
* @param[in] header Pointer to window in which the header will be written.
* @param[in] highlight Index of option which is selected.
* @param[in] title String that will be written as program title.
*/
void writeHeaderMenu(WINDOW *header, int highlight, char *title);

/**
* @brief Function write the footer of the program.
* The footer will also contain the message passed as argument message.
* @param[in] footer Window in which the footer message will be written.
* @param[in] message Message that will be written in the footer.
*/
void writeFooterMenu(WINDOW *footer, char *message);

/**
* @brief Function that is used to initialize the color scheme used by the rest of the program.
*/
void initColorScheme();

/**
* @brief Function writes the message in the window where the user can respond with YES / NO answer.
* The message should not be longer than 30 characters.
* @param[in] prompt Pointer to window in which the message will be written.
* @param[in] highlight Index of marked answer (YES or NO).
* @param[in] mesg String that contains the message to be written.
*/
void promptText(WINDOW *prompt, int highlight, char *mesg);

/**
* @brief Function deletes the window, before that it colors it with color passed as argument.
* @param[in] win Pointer to window to be deleted.
* @param[in] scheme Color scheme that the window will be colored with before it will be deleted.
*/
void destroyWin(WINDOW *win, chtype scheme);

/**
* @brief Function opens the message in smaller YES / NO window and returns users answer.
* @param[in] mesg The message written in the window.
* @return int If the user confirms YES it returns 1, otherwise 0.
*/
int prompt(char *mesg);

/**
* @brief Function reads the name of the archive in which the files will be saved and compressed.
* @param[in] fp Pointer to the structure FilePaths in which the archive name will be saved.
* @param[in] footer Pointer to window in which the user will enter the archive name.
*/
void archiveName(struct FilePaths *fp, WINDOW *footer);

//compressionCalls.c
/**
* @brief Function calls coresponding functions for compression.
* @param[in] fp Structure FilePaths which contains paths to selected files.
* @param[in] log Flag that marks if the log file should be maintained.
* @param[in] algorithm Flag that marks which algorithm for compression should be used.
*/
void callCompression(struct FilePaths fp, int log, int algorithm);

//decompressionCalls.c
/**
* @brief Function calls coresponding functinos for decompression.
* @param[in] fp Structure FilePaths which contains paths to archives.
* @param[in] log Flag that marks if the log file should be maintained.
*/
void callDecompression(struct FilePaths fp, int log);

//fileManagment.c
/**
* @brief Function that checks if the file in filepath can be read.
* @param[in] path Path to file that should be checked for reading.
* @return int It returns 1 if the file can be read, 0 otherwise.
*/
int isReadable(char *path);

/**
* @brief Function adds to the structure Listing the name of the file that is passed.
* @param[out] list Pointer to structure Listing in which the filepath will be added to.
* @param[in] file Pointer to structure dirent of the file that should be added to Listing.
*/
void addListing(struct Listing *list, struct dirent *file);

/**
* @brief Function adds path to file to structure FilePaths.
* @param[out] fp Pointer to structure FilePaths in which the path will be added.
* @param[in] path String that contains filepath that should be added.
*/
void addFilePath(struct FilePaths *fp, char *path);

/**
* @brief Function checks if the path is not inside the structure FilePaths.
* @param[in] fp Structure FilePaths which will be searched.
* @param[in] name String that will be searched for in structure FilePaths.
* @return int If paths has not been find the function return 1, otherwise 0.
*/
int notIn(struct FilePaths fp, char *name);

/**
* @brief Function adds checked files from structure Listing to list of filepaths in structure FilePaths.
* @param[in] list Structure Listing from which the marked files will be added.
* @param[out] fp Pointer to structure FilePaths in which the marked files will be added.
*/
void addChecked(struct Listing list, struct FilePaths *fp);

/**
* @brief Function deletes files that are not checked in structure Listing, but are present in the structure FilePaths.
* @param[in] list Structure Listing from which the filepaths to delete will be read.
* @param[out] fp Pointer to structure FilePaths from which the filepaths that are not checked in Listing will be deleted.
*/
void deleteUnchecked(struct Listing list, struct FilePaths *fp);

/**
* @brief Function marks files in structure Listing if they are in list of filepaths in structure FilePaths.
* @param[out] list Pointer to structure Listing in which the files will be checked.
* @param[in] fp Structure FilePaths which contains paths to checked files.
*/
void markChecked(struct Listing *list, struct FilePaths fp);

/**
* @brief Function writes the name of files and directories from structure Listing to window body.
* File that has the 'cursor' on it will be marked red.
* @param[in] body Pointer to window in which the files will be listed.
* @param[in] list Structure from which the names of files and directories will be read.
* @param[in] highlight Index of the selected item.
* @return int Index of the last file from structure Listing that was written in window body.
*/
int writeListing(WINDOW *body, struct Listing list, int highlight);

/**
* @brief Function deallocates all memory that was allocated for storing the names of files.
* @param[out] list Pointer to structure whose memory will be deallocated.
*/
void deleteListing(struct Listing *list);

/**
* @brief Functoin writes all files that are in structure FilePaths.
* That structure contains paths to all files that the user has selected up until the call of the function.
* @param[in] body Pointer to window in which the paths to files should be written.
* @param[in] fp Structure FilePaths which contains list of all paths to the selected files.
*/
void printCheckedFiles(WINDOW *body, struct FilePaths fp);

/**
* @brief This function realizes a menu in which the user can mark files for compression / decompression
* and move through diferent directories to view and selected other files.
* @param[in] body Window in which the files will be listed.
* @param[in] ind This argument can be 0 (to list all files) or 1 (to list only archives that can be decompressed).
* @return struct FilePaths Structure that contains strings that contain full paths to selected files.
*/
struct FilePaths fileSelection(WINDOW *body, int ind, WINDOW *footer);

/**
* @brief Function reads the names of all files and folders in folder paths to structure Listing.
* @param[out] list Pointer to structure Listing in which the names of files and folders will be stored.
* @param[in] path String that contains path to folder that will be read.
*/
void listFiles(struct Listing *list, char *path);

/**
* @brief Function that checks if the file has archive extension defined in menuHeader.h
* @param[in] arch Structure dirent of file to be checked.
* @return int Returns 1 if the file has archive extension, otherwise 0.
*/
int isArchive(char *arch);

/**
* @brief Function reads the names of all archives and folders which are in folder path.
* @param[out] list Pointer to structure Listing in which all archives and folders will be written to.
* @param[in] path String which contains the path to folder from which the archives and folders will be read.
*/
void listArchives(struct Listing *list, char *path);

//commandLine.c
/**
* @brief Function adds paths to files passed through command lines into structure FilePaths.
* @param[in] argc Number of command line arguments.
* @param[in] argv Command line arguments.
* @param[out] fp Pointer to structure FilePaths in which paths to files will be added.
* @param[in] i Index of first argument from which the names of files start.
* @param[in] archives Flag that says should the files selected be checked for type (Are they archives?).
*/
void addPathsCMD(int argc, char *argv[], struct FilePaths *fp, int i, int archives);

/**
* @brief Funtion checks if the string matches the pattern with * and ? wildcards.
* @param[in] str String that is being checked.
* @param[in] ptr Pattern that contains * and ? against which the string will be compared.
* @return int If string mathces the pattern function returns 1, otherwise 0.
*/
int isMatch(char *str, char *ptr);

/**
* @brief Function checks if string p is a full path to a file.
* @param[in] p String that is being checked.
* @return int Returns 1 if string p is full path to file, otherwise 0.
*/
int fullPath(char *p);

/**
* @brief Function checks if the argument passed through command line contains wildcards.
* @param[in] p String that is being checked.
* @return int Returns 1 if string doesn't contain wildcards, otherwise 0.
*/
int isPath(char *p);

/**
* @brief Function writes help instruction how to use the program from the command line.
* @param[in] path String that contains the path to program.
* This argument is used to write the programs name.
*/
void writeHelp(char *path);

/**
* @brief Function writes About section of the program when the command line interface is used.
*/
void writeAbout();

/**
* @brief Function writes an error message when the program is used from the command line.
*/
void writeError();

#endif
