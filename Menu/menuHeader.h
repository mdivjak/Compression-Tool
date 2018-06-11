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
* @brief Funkcija pokrece graficki tj. Text User Interface (TUI).
* Funkcija omogucava korisniku da izabere neku od opcija programa.
* Nakon toga se pokrece meni za selekciju fajlova i potom se putanje
* do tih fajlova prosledjuju funkcijama za kompresiju / dekompresiju.
*/
void startTUI();

/**
* @brief Funkcija koja se poziva prilikom interfejsa preko komandne linije.
* @param[in] argc Broj argumenata prosledjenih preko komandne linije.
* @param[in] argv Niz stringova argumenata koji su prosledjeni putem komandne linije.
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
* @brief Funkcija ispisuje prozor u kome korisnik bira koji
* algoritam ce se koristiti prilikom kompresovanja.
* @return int Vraca indeks algoritma koji se koristi za kompresiju.
*/
int selectAlgorithm();

/**
* @brief Funkcija ispisuje ponudjene algoritme u prozor algo
* i markira selektovani izbor sa indeksom highlight
* @param[in] algo Pokazivac na prozor u kome se ispisuje lista algoritama za selekciju.
* @param[in] highlight Indeks opcije koja je trenutno selektovana.
*/
void promptAlgorithms(WINDOW *algo, int highlight);

/**
* @brief Funkcija ispisuje zaglavlje programa u kome se nalazi
* naslov i opcije koje korisnik moze da izabere.
* @param[in] header Prozor u okviru kog se vrsi ispis.
* @param[in] highlight Indeks opcije koja je oznacena tamnijom bojom.
* @param[in] title String koji se ispisuje kao naslov programa.
*/
void writeHeaderMenu(WINDOW *header, int highlight, char *title);

/**
* @brief Funkcija ispisuje footer programa u kome se ispisuje
* poruka koja je prosledjena funkciji.
* @param[in] footer Prozor u okviru kog se vrsi ispis.
* @param[in] message Poruka korisniku koja se ispisuje u footeru.
*/
void writeFooterMenu(WINDOW *footer, char *message);

/**
* @brief Funkcija sluzi za inicijalizaciju seme boja koje koristi program.
*/
void initColorScheme();

/**
* @brief Funkcija ispisuje poruku u prozoru za odabir YES / NO odgovora.
* Poruka ne bi trebala da bude duza od 30 karaktera.
* @param[in] prompt Prozor u okviru kog se vrsi ispis.
* @param[in] highlight Indeks odgovora koji je markiran (YES ili NO).
* @param[in] mesg String koji sadrzi poruku koja se ispisuje.
*/
void promptText(WINDOW *prompt, int highlight, char *mesg);

/**
* @brief Funkcija brise prozor, a prethodno ga boji prosledjenom semom boja.
* @param[in] win Prozor koji se brise.
* @param[in] scheme Sema kojom se boji prozor pre nego sto se obrise.
*/
void destroyWin(WINDOW *win, chtype scheme);

/**
* @brief Funkcija otvara poruku u manjem YES / NO prozoru i vraca odgovor korisnika.
* @param[in] mesg Poruka koja se ispisuje u prozoru.
* @return int Ukoliko korisnik potvrdi YES vraca 1, u suprotnom 0.
*/
int prompt(char *mesg);

/**
* @brief Funkcija ucitava ime arhive u kojoj ce se sacuvati kompresovani fajlovi.
* @param[in] fp Pokazivac na strukturu FilePaths u kojoj se cuva ime arhive u polju fp->archive.
* @param[in] footer Pokazivac na prozor u okviru kog se trazi od korisnika da unese ime fajla.
*/
void archiveName(struct FilePaths *fp, WINDOW *footer);

//compressionCalls.c
/**
* @brief Funkcija poziva odgovarajuce funkcije za kompresiju.
* @param[in] fp Struktura FilePaths u kojoj se nalaze putanje do fajlova.
* @param[in] log Flag koji oznacava da li treba praviti log fajl.
* @param[in] algorithm Flag koji oznacava koji algoritam treba koristiti.
*/
void callCompression(struct FilePaths fp, int log, int algorithm);

//decompressionCalls.c
/**
* @brief Funkcija poziva odgovarajucu funkciju za dekompresiju.
* @param[in] fp Struktura FilePaths u kojoj se nalaze putanje do fajlova.
* @param[in] log Flag koji oznacava da li treba praviti log fajl.
*/
void callDecompression(struct FilePaths fp, int log);

//fileManagment.c
/**
* @brief Funkcija proverava da li je moguce citanje fajla sa putanjom path.
* @param[in] path Putanja do fajla za koji se proverava da li je moguce citanje.
* @return int Ukoliko je moguce da se fajl procita vraca 1, u suprotnom 0.
*/
int isReadable(char *path);

/**
* @brief Funkcija dodaje u strukturu Listing ime fajla koji je prosledjen.
* @param[out] list Pokazivac na strukturu Listing u koju ce se dodati ime fajla.
* @param[in] file Pokazivac na strukturu dirent fajla koji se dodaje u listing fajlova.
*/
void addListing(struct Listing *list, struct dirent *file);

/**
* @brief Funkcija dodaje putanju do fajla u strukturu FilePaths.
* @param[out] fp Pokazivac na strukturu FilePaths u koju se dodaje putanja do fajla.
* @param[in] path String u kome se nalazi putanja do fajla koji se dodaje u strukturu.
*/
void addFilePath(struct FilePaths *fp, char *path);

/**
* @brief Funkcija proverava da li se putanja ne nalazi u strukturi FilePaths.
* @param[in] fp Struktura FilePaths u kojoj se pretrazuju putanje do fajlova.
* @param[in] name String u kome se nalazi putanja koja se trazi u strukturi fp.
* @return int Ukoliko putanja nije pronadjena u strukturi vraca 1, u suprotnom 0.
*/
int notIn(struct FilePaths fp, char *name);

/**
* @brief Funkcija dodaje cekirane fajlove iz strukture Listing u listu
* putanja fajlova u strukturi FilePaths.
* @param[in] list Struktura Listing iz koje se dodaju markirani fajlovi.
* @param[out] fp Pokazivac na strukturu FilePaths u koju se dodaju
* putanje do markiranih fajlova.
*/
void addChecked(struct Listing list, struct FilePaths *fp);

/**
* @brief Funkcija brise fajlove koji nisu cekirani u strukturi Listing,
* ali se nalaze u listi putanja u strukturi FilePaths.
* @param[in] list Struktura Listing iz koje se listaju putanje fajlova koje se brisu.
* @param[out] fp Pokazivac na strukturu FilePaths iz koje se brisu putanje
* koje se nalaze u listingu i nisu cekirane.
*/
void deleteUnchecked(struct Listing list, struct FilePaths *fp);

/**
* @brief Funkcija cekira fajlove u strukturi Listing, ako se oni nalaze u listi
* putanja u strukturi FilePaths.
* @param[out] list Pokazivac na strukturu Listing u kojoj se cekiraju fajlovi.
* @param[in] fp Struktura FilePaths koja sadrzi putanje cekiranih fajlova.
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
