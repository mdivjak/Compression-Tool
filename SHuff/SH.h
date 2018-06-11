#ifndef SH_INCLUDED
#define SH_INCLUDED

#define  ASCII_SH 256

#include <stdio.h>
#include <stdlib.h>

/*
*@brief:Stablo za generisanje kodova
*@param:znak vrednost sifriranog znaka ukoliko je cvor list
*@param:v tezina cvora
*@param:*l,*d deca
*@param:list da li je cvor list
*/
typedef struct shuffman StaticHuffmanTree;
/*
@brief:Prioritetni red cvorova stabla
*/
typedef struct priority_queue PriorityQueue;

/*
*@brief:sifra, koja se vezuje uz cvora
*@param:v vrednost (u decimalnom zapisu)
*@param:br_bitova vroj bitova iz kojih se sastoji
*/
typedef struct sifra Sifra;

/*
*brief:Red cvorova, sluzi za level-order pretrage
*/
typedef struct sh_queue SH_Queue;

typedef struct par Par;

/*
*@brief:Da li je red prazan
*@param[in]: front pocetak reda 
*@return 0 ako jeste nesto drugo inace
*/
int sh_queue_empty(SH_Queue *front);
/*
*@brief: Stavlja cvor na pocetak reda
*@param[out]:front prvi element
*@param[out]:rear zacelje reda
*@param[out]:cvor cvor koji se ubacuje
*@param[out]:sifra sifra cvora (odgovara putu od korena do njega)
*/
void sh_queue_push(SH_Queue **front, SH_Queue **rear,StaticHuffmanTree *cvor,Sifra code);
/*
*@brief: Izbacuje prvi element iz reda
*@param[out]:front prvi element
*@param[out]:rear zacelje reda
*/
void sh_queue_pop(SH_Queue **front, SH_Queue **rear);

/*
*@brief: da li je cvor list
*@param[in]:cvor
*@return 0 ako jeste nesto drugo inace
*/
int leaf(StaticHuffmanTree *cvor);
/*
*@brief: pravi cvor cija je tezina p[ind]
*@param[in]:p 
*@param[in]:ind
*@return cvor
*/
StaticHuffmanTree *get_node(int p[],int ind);
/*
*@brief: dodeljuje oca dvama cvorovima huffmanovog stabla
*@param[in]:x prvi cvor 
*@param[in]:y prvi cvor
*@param[in]:tezina novog cvora
*@return novi cvor-otac
*/
StaticHuffmanTree *merge(StaticHuffmanTree **x,StaticHuffmanTree **y,int v);
/*
*@brief:ispisuje sifru odredjenog znaka
*@param[in]:koren koren Huffmanovog  stabla
*@param[in]:znak trazeni znak, sigurno postoji
*@return sifra
*/
Sifra sh_output(StaticHuffmanTree *koren,unsigned char znak);

/*
*@brief:inicijalizuje prioritetni red
*@param[out]:pq prioritetni red
*@param[in]:znak trazeni znak, sigurno postoji
*@return sifra
*/
void pq_init(PriorityQueue *pq);
/*
*@brief:da li je pr. red prazan
*@param[in]:pq
*@return 0 ako nije nesto drugo inace 
*/
int pq_empty(PriorityQueue pq);
/*
*@brief:vraca cvor najmanje tezine
*@param[in]:pq
*@return cvor najmanje tezine
*/
StaticHuffmanTree *pq_front(PriorityQueue pq);
void pq_push(PriorityQueue *pq,StaticHuffmanTree *novi);
void pq_pop(PriorityQueue *pq);
/*
*@brief:pravi H. stablo
*@param[in]:p[] broj pojavljivanja svakog znaka
*@return koren stabla
*/
StaticHuffmanTree *build_SHTree(int p[]);
/*
@brief: ispisuje sifru
*/
void ispisi_sifru(Sifra s);
/*
*@brief:broji pojavljivanja svakog znaka u datoteci
*@param[in]:in ulazni fajl
*@param[out]:p[] broj pojavljivanja svakog znaka
*/
void prebroj_verovatnoce_pojavljivanja(FILE *in,int p[]);
/*
*@brief:postavlja broj pojavljivanja svakog znaka na 0
*@param[out]:p[] broj pojavljivanja svakog znaka
*/
void SH_initp(int p[]);
/*
*@brief:stavlja indeks algoritma kompresovanja u datoteku
*@param[out]:out izlazna datoteka
*/
void ispisi_indeks_datoteke(FILE *out);
/*
*@brief:vraca prvih k bitova date sifre
*@param[in]:s sifra
*@param[in]:k
*@return: prvih k bitova
*/
unsigned char prvih_k(Sifra s,int k);
/*
*@brief:odseca prvih k bitova date sifre
*@param[in]:s sifra
*@param[in]:k
*@return:vrednost broj koji ostaje 
*/
unsigned short odseci_k(Sifra s,int k);
/*
*@brief:upisuje bajt na kraj datoteke
*@param[in]:vis redudantni bitovi (potrebni radi celog broja bajtova)
*@param[out]:out izlazna datoteka
*/
void na_kraj_datoteke(unsigned char vis,FILE *out);
/*
*@brief:upisuje brojeve pojavljivanja svakog znaka u datoteku, kako bi dekompresor pravio stablo
*@param[in]:p[] brojevi pojavljivanja znakova
*@param[out]:out izlazna datoteka
*/
void ispisi_verovatnoce(int p[],FILE *out);
/*
*@brief:kompresuje datoteku
*@param[in]:in ulazna datoteka
*@param[out]:out izlazna datoteka
*/
void SH_basic_compression(FILE *in,FILE *out);
/*
*@brief:preskace indeks kompresije
*@param[in]:in ulazna datoteka
*/
void ucitaj_indeks(FILE *in);
/*
*@brief:ucitava brojeve pojavljivanja svakog znaka
*@param[in]:in ulazna datoteka
*@param[out]:p[] brojevi pojavljivanja znakova
*/
void ucitaj_verovatnoce(FILE *in,int p[]);
/*
*@brief:ucitava poslednji bajt tj. koliko je redudantnih bitova
*@param[in]:in ulazna datoteka
*/
unsigned char poslednji_bajt(FILE *in);
/*
*@brief:vraca broj bitova koji su deo sifre
*@param[in]:in ulazna datoteka
*@param[in]:vis broj redudantnih bajtova
*@return broj bitova koji su deo sifre
*/
int posl_smislen_bit(FILE * in,unsigned char vis);

/*
*@brief:dekompresuje datoteku
*@param[in]:in ulazna datoteka
*@param[in]:out izlazna datoteka
*/
void SH_basic_decompression(FILE *in,FILE *out);
#endif
