#ifndef LZW_INCLUDED
#define LZW_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define ASCII 257
#define VELICINA 4080
#define DUZINA 10

/*
*@brief: trie koji sluzi kao kompresiona tabela
*@param: sifra sifra reci koja se ubacuje u trie
*@param: *pok sinovi cvora u trie-u, put izmedju korena i ssvakog nenultog cvora u trie-u predstavlja rec u tabeli, u pok su smestene grane tako da ako 
ako je pokazivac na cvor 'v' na mestu pok[i] u cvoru 'u', to znaci da se rec od korena do 'v' dobija dodavanjem slova sa ascii kodom i na rec od korena do 'u'
*/
typedef struct trie Trie;
/*
*@brief:Dekompresiona tabela
*@param:table[][] niz reci gde table[kod] predstavlja rec sa kodom sif
*@param:duz broj reci trenutno u tabeli
*@param l[] duzine odgovarajucih reci
*/
typedef struct dcmp_table decompression_table;

/*
*@brief:stek cvorova trie-a oranizovan kao ulancana lista
*/
typedef struct stack Stack;
/*
@brief: provera da li je stek prazan
@param[in]: top pokazivac na vrh steka
@return: 1 ako je prazan 0 inace
*/
int empty(Stack *top);

/*
@brief: ubacivanje elementa u stek
@param[out]: top pokazivac na vrh steka u koji se dodaje
*/
void push(Stack **top,Trie *cvor);
/*
@brief: izbacivanje elementa sa vrha steka
@param[out]: top pokazivac na vrh steka
*/
void pop(Stack **top);

/*
*@brief: inicijalizuje novi cvor u trie-u
*@param[in]:sif sifra novog cvora
*/
Trie *getnode(int sif);
/*
*@brief: inicijalizuje kompresionu tabelu
*@param[in]:koren koren trie-a koji predstavlja tabelu
*@return pokazivac na koren nove tabele 
*/
Trie *init(Trie *koren);
/*
*@brief: Ubacuje datu rec u tabelu
*@param[in]:koren koren trie-a koji predstavlja tabelu
*@param[in]:rec
*@param[in]:sifr sifra/kod reci koja se ubacuje u tabelu
*@param[in]:n duzina reci
*@return pokazivac na koren nove tabele 
*/
Trie *insert(Trie *koren,unsigned short rec[],int sifra,int n);
/*
*@brief: Utvrdjuje postoji li data rec u tabeli
*@param[in]:koren koren trie-a koji predstavlja tabelu
*@param[in]:rec
*@param[in]:duzina trazene reci
*@return 1 ako se rec nalazi u tabeli, 0 u suprotnom 
*/
int contains(Trie *koren,unsigned short rec[],int);
/*
*@brief: Funkcija ispisuje kod reci za koju znamo da se nalazi u tabeli
*@param[in]:koren Koren trie-a koji predstavlja tabelu
*@param[in]:rec
*@param[in]:duzina trazene reci
*@return: kod date reci
*/
unsigned short output(Trie *koren,unsigned short rec[],int n);
/*
*@brief:Funkcija koja oslobadja kompresionu tabelu
*@param[out: koren pokazivac na koren trie-a koji pretstavlja kompresionu tabelu
*/
void clear_trie(Trie **koren);

/*
*@brief: Sabira rec i karakter i rezultat cuva u zbiru; simulira string+character
*@param[out]: zbir[] rec koja se dobija
*@param[in]: s1[] rec koja se sabira
*@param[in]: s2 karakter koji se dodaje
*@param[out]: n duzina reci na koju se dodaje karakter (duzina bira bice za 1 veca)
*/
void saberi(unsigned short zbir[],unsigned short s1[],unsigned char s2,int n);
/*
*@brief: Upisuje umesto s1 rec s2 koja je nastala dodavanjem jednog karaktera na s1; simulira string=string+character
*@param[out]: s1[] niz u kome uvamo novu rec
*@param[in]: s2[] rec na koju prepisujemo
*@param[out]: *n duzina nove reci
*/
void dodeli(unsigned short s1[],unsigned short s2[],int *n);
/*
*@brief: Prepisuje jednu rec u drugu
*@param[out]: s1[] niz u koji upisujemo rec
*@param[in]: s2[] niz iz koga uzimamo
*@param[in]: m duzina reci koju uzimamo
*@param[out]: n duzina stare reci kojoj sada dodeljujemo vrednost nove
*/
void dodeli_razlcitih_duzina(unsigned short s1[],unsigned short s2[],int *n,int m);
/*
*@brief: Postavlja rec na jedan karakter tj. vrsi naredbu string=char u lzw-u
*@param[out]: s1[] niz u kome uvamo novu rec
*@param[in]: karakter na ciju vrednost postavljamo rec
*@param[out]: *n duzina nove reci
*/
void dodeli_char(unsigned short s1[],unsigned char c,int *n);

/*
*@brief: Funkcija vrsi kompresiju po ulazne datoteke u izlaznu koriscenjem lzw algoritma
*@param[in]: in pokazivac na ulaznu datoteku
*@param[out]: in pokazivac na izlaznu datoteku
*/
void lzw_basic_compression(FILE *in,FILE *out);

/*
*@brief: Funkcija inicijalizuje dekompresionu tabelu
*@param[out] dcmp dekompresiona tabela
*/
void init_decompression_table(decompression_table *dcmp);
/*
*@brief: Funkcija utvrdjuje da li se kod nalazi u tabeli
*@param[in] dcmp dekompresiona tabela
*@param[in] ind kod za koji se utvrdjuje da li je u tabeli
*/
int contains_dcmp(decompression_table dcmp,unsigned short ind);
/*
*@brief: Funkcija upisuje polje oznaceno odgovarajucim kljucem iz tabele u niz
*@param[out] pom_isp[] niz u koji se upisuje vrednost reci
*@param[in] dcmp dekompresiona tabela
*@param[in] ind kod reci
*@param[out] n duzina dobijene reci
*/
void translate(unsigned short pom_isp[],decompression_table dcmp,unsigned short ind,int *n);
/*
*@brief: Funkcija dodaje novu rec u dekompresionu tabelu
*@param[out] dcmp dekompresiona tabela
*@param[in] rec[] nova rec koja se upisuje u tabelu
*@param[in] n broj karaktera u reci
*/
void table_add(decompression_table *dcmp,unsigned short rec[],int n);
/*
*@brief: Funkcija brise vrednosti dodate u dekompresionu tabelu
*@param[out] *dcmp dekompresiona tabela
*/
void clear_dcmp(decompression_table *dcmp);
/*
*@brief: Funkcija ispisuje niz unsigned shortova u datoteku
*@param[in] pom_isp niz koji se ispisuje
*@param[in] n duzina niza
*@param[out] izlazna datoteka u koju se upisuje
*/
void ispisi(unsigned short pom_isp[],int n,FILE *out);
/*
@brief: Ispisuje prva cetiri bita iz bajta
*/
unsigned short prva_cetiri(unsigned char c);
/*
@brief: Ispisuje poslednja cetiri bita iz bajta
*/
unsigned short poslednja_cetiri(unsigned char c);
/*
*@brief: Funkcija dekompresuje datoteku kompresovanu koriscenjem lzw-a
*@param[in] in pokazivac na ulaznu, kompresovana datoteku
*@param[out] preko out se vraca izalzna datoteka
*/
void lzw_basic_decompression(FILE *in,FILE *out); 
#endif
