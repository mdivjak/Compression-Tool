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
* @brief Funkcija ispisuje imena fajlova i direktorijuma iz listinga na prozor body.
* Crvenom bojom oznacava fajl / direktorijum na kome se trenutno nalazi 'kursor'.
* @param[in] body Prozor u okviru kog se realizuje ispis.
* @param[in] list Struktura iz koje se citaju i ispisuju imena fajlova i direktorijuma.
* @param[in] highlight Indeks stavke koja je oznacena 'kursorom' tj. crvenom bojom.
* @return int Indeks poslednjeg fajla iz strukture koji je ispisan na prozoru body.
*/
int writeListing(WINDOW *body, struct Listing list, int highlight);

/**
* @brief Funkcija dealocira svu memoriju koja je alocirana za cuvanje imena foldera
* tj. fajlova u strukturi. Postavlja sve promenljive na 0 tj. NULL za pokazivace.
* @param[out] list Pokazivac na strukturu iz koje se dealocira memorija.
*/
void deleteListing(struct Listing *list);

/**
* @brief Funkcija ispisuje sve fajlove koji se nalaze u strukturi FilePaths.
* U toj strukturi se nalaze putanje do svih fajlova koje je korisnik markirao
* do trenutka poziva funkcije.
* @param[in] body Prozor u okviru kog treba ispisati listu putanja do fajlova.
* @param[in] fp Struktura FilePaths u kojoj se nalazi lista svih putanja do
* markiranih fajlova.
*/
void printCheckedFiles(WINDOW *body, struct FilePaths fp);

/**
* @brief U okviru ove funkcije se realizuje meni u kome korisnik moze
* da obelezi fajlove za kompresiju / dekompresiju kao i da se krece kroz
* razlicite direktorijume.
* @param[in] body Prozor u kome se realizuje ispis svih fajlova i kretanje
* kroz meni, kao i odabir fajlova koji ce se kompresovati / dekompresovati.
* @param[in] ind Ovaj argument funkcije moze biti 0 (ako se biraju fajlovi
* za kompresiju) ili 1 (ako se biraju arhive za dekompresiju). Ovim argumentom
* se bira da li ce se pozivati funkcije za listing fajlova ili
* funkcije za listing arhiva.
* @return struct FilePaths Struktura koja sadrzi stringove u kojima se nalaze
* putanje do izabranih fajlova za kompresiju / dekompresiju.
*/
struct FilePaths fileSelection(WINDOW *body, int ind, WINDOW *footer);

/**
* @brief Funkcija u strukturu Listing ucitava imena svih fajlova
* i foldera koji se nalaze u folderu path.
* @param[out] list Pokazivac na strukturu Listing u koju se
* upisuju sva imena fajlova i foldera u putanji path.
* @param[in] path String koji je putanja do foldera iz kog
* se ucitavaju fajlovi i folderi.
*/
void listFiles(struct Listing *list, char *path);

/**
* @brief Funkcija koja proverava da li fajl ima ekstenziju
* arhive definisanu u menuHeader.h.
* @param[in] arch Struktura dirent fajla koji se proverava.
* @return int Ako fajl ima ekstenziju arhive vraca 1, u suprotnom 0.
*/
int isArchive(char *arch);

/**
* @brief Funkcija u strukturu Listing ucitava imena svih arhiva
* i foldera koji se nalaze u folderu path.
* @param[out] list Pokazivac na strukturu Listing u koju se
* upisuju sva imena arhiva i foldera u putanji path.
* @param[in] path String koji je putanja do foldera iz kog
* se ucitavaju arhive i folderi.
*/
void listArchives(struct Listing *list, char *path);

//commandLine.c
/**
* @brief Funkcija dodaje putanje fajlova prosledjenih u komandnoj liniji, u strukturu FilePaths.
* @param[in] argc Broj argumenata komandne linije.
* @param[in] argv Argumenti komandne linije.
* @param[out] fp Pokazivac na strukturu FilePaths u koju se dodaju putanje do fajlova.
* @param[in] i Indeks argumenta od kog pocinju putanje fajlova.
* @param[in] archives Flag koji oznacava da li treba proveravati da li su selektovani fajlovi arhive.
*/
void addPathsCMD(int argc, char *argv[], struct FilePaths *fp, int i, int archives);

/**
* @brief Funkcija proverava da li string odgovara obrascu sa wildcardovima * i ?.
* @param[in] str String koji se proverava.
* @param[in] ptr Obrazac koji ukljucuje * i ? sa kojim se uporedjuje string.
* @return int Ukoliko string odgovara obrascu vraca 1, u suprotnom 0.
*/
int isMatch(char *str, char *ptr);

/**
* @brief Funkcija proverava da li je string p puna putanja do nekog fajla.
* @param[in] p String koji se proverava.
* @return int Ukoliko je p puna putanja do fajla vraca 1, u suprotnom 0.
*/
int fullPath(char *p);

/**
* @brief Funkcija proverava da li je argument koji je prosledio korisnik preko
* komandne linije putanja do fajla, ili sadrzi specijalne znake * i ?
* @param[in] p String koji se proverava.
* @return int Vraca 1 ako putanja ne sadrzi znake za selekciju, 0 u suprotnom.
*/
int isPath(char *p);

/**
* @brief Funkcija ispisuje objasnjenje za koriscenje programa
* preko komandne linije.
* @param[in] path String u kome se nalazi putanja do .exe fajla.
* Ovaj argument sluzi da funkcija ispise tacno ime programa.
*/
void writeHelp(char *path);

/**
* @brief Funkcija ispisuje About sekciju prilikom koriscenja
* programa preko komandne linije.
*/
void writeAbout();

/**
* @brief Funkcija koja prijavljuje gresku u argumentima prilikom
* koriscenjem programa preko komandne linije.
*/
void writeError();

#endif
