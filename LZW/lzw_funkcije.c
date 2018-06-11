#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_LZW 257
#define VELICINA_LZW 4080
#define DUZINA_LZW 10

//////////////////////////
//REGION: STURKTURE PODATKA
//////////////////////////  

typedef struct trie
{
	unsigned short sifra;
	struct trie *pok[ASCII_LZW];
}Trie;

typedef struct dcmp_table
{
	unsigned short table[VELICINA_LZW][DUZINA_LZW];
	int duz;
	unsigned short l[VELICINA_LZW];
}decompression_table;

typedef struct stack
{
	Trie *v;
	struct stack *sl;
}Stack;

////////////
//REGION:STACK
/////////////

int empty(Stack *top)
{
	return (top==NULL);
}

void push(Stack **top,Trie *cvor)
{
	Stack *pom=malloc(sizeof(Stack));
	pom->v=cvor;
	pom->sl=(*top);
	*top=pom;
}

void pop(Stack **top)
{
	Stack *pom=(*top);
	(*top)=(*top)->sl;
	free(pom);
}

//////////////
//REGION: TRIE
//////////////

Trie *getnode(int sif)
{
	Trie *novi=(Trie *)malloc(sizeof(Trie));
	novi->sifra=sif;
	int i;
	for (i=0;i<ASCII_LZW;i++)
	{
		novi->pok[i]=NULL;
	}
	return novi;
}

Trie *init(Trie *koren)
{
	koren=getnode(-1);
	int i;
	for (i=0;i<ASCII_LZW;i++)
	{
		Trie *novi=getnode(i);
		koren->pok[i]=novi;
	}
	return koren;
}

Trie *insert(Trie *koren,unsigned short rec[],int sifra,int n)
{
	//u lzw-u ce se uvek dodavati na nesto postojece
	int i;
	unsigned char slovo;
	Trie *tr=koren;
	for (i=0;i<n-1;i++)
	{
		slovo=rec[i];
		tr=tr->pok[slovo];
	}
	Trie *novi=getnode(sifra);
	tr->pok[rec[i]]=novi;
	return koren;
}

int contains(Trie *koren,unsigned short rec[],int n)
{
	int i;
	unsigned char slovo;
	Trie *tr=koren;
	for (i=0;i<n;i++)
	{
		slovo=rec[i];
		if (!tr->pok[slovo])
			return 0;
		tr=tr->pok[slovo];
	}
	return 1;
}

unsigned short output(Trie *koren,unsigned short rec[],int n)
{
	int i;
	unsigned char slovo;
	Trie *tr=koren;
	for (i=0;i<n;i++)
	{
		slovo=rec[i];
		tr=tr->pok[slovo];
	}
	return tr->sifra;
}

void clear_trie(Trie **koren)
{
	Trie *tek;
	Stack *top=malloc(sizeof(Stack));
	top=NULL;
	push(&top,*koren);
	while (!empty(top))
	{
		tek=top->v;
		pop(&top);
		int i;
		for (i=0;i<ASCII_LZW;i++)
		{
			if (tek->pok[i])
				push(&top,tek->pok[i]);
			
		}
		free(tek);
	}
	*koren=NULL;
}
//////////////////////////////////
//REGION: STRINGOVSKA MANIPULACIJU
//////////////////////////////////

void saberi(unsigned short zbir[],unsigned short s1[],unsigned char s2,int n)
{
	int i=0;
	n++;
    for (i=0;i<n-1;i++)
        zbir[i]=s1[i];          
	zbir[n-1]=s2;
}

void dodeli(unsigned short s1[],unsigned short s2[],int *n)//razlikuju se za karakter
{

	(*n)++;
    s1[*n-1]=s2[*n-1];
}

void dodeli_razlcitih_duzina(unsigned short s1[],unsigned short s2[],int *n,int m)
{

	int i,j;
	for (i=0;i<m;i++)
	{
		s1[i]=s2[i];
	}
	*n=m;
}

void dodeli_char(unsigned short s1[],unsigned char c,int *n)
{
    *n=1;
    s1[0]=c;
}
//METODE:

void lzw_basic_compression(FILE *in,FILE *out)
{
	Trie *table = NULL;
	table = init(table);
	unsigned short sif=ASCII_LZW,pom_short;
	unsigned short string_code[VELICINA_LZW],zbir[VELICINA_LZW];
	int duz=0,zlen=0;
	unsigned char character;
	unsigned char next;
	unsigned char isp1,isp2,br=1;
	fread(&next,sizeof(char),1,in);

	char algoCode = 2;
	fwrite(&algoCode, 1, 1, out);

	string_code[duz++]=next;  
	while (fread(&next,sizeof(char),1,in)!=0)
	{
		character=next;
		saberi(zbir,string_code,character,duz);
		zlen=duz+1;
		if (contains(table,zbir,duz+1))
		{
			dodeli(string_code,zbir,&duz);//string_code postaje zbir;
		}
		else
		{
			unsigned short isp=output(table,string_code,duz);
			if (br==1)
			{
				br=2;
				pom_short=(isp>>4);
				isp1=(unsigned char)pom_short;
				fwrite(&isp1,sizeof(char),1,out);
				pom_short=isp&15;//poslednja cetiri bita
			}
			else
			{
				br=1;
				pom_short=(pom_short<<12)+isp;
				isp1=(pom_short>>8);
				fwrite(&isp1,sizeof(char),1,out);
				isp1=pom_short&((1<<8)-1);
				fwrite(&isp1,sizeof(char),1,out);
			}
			//izmena! ovde dodato && zlen<DUZINA_LZW
			if (sif+1<=VELICINA_LZW && zlen<DUZINA_LZW)
				insert(table,zbir,sif++,zlen);
			dodeli_char(string_code,character,&duz);
		}
	}
	unsigned short isp=output(table,string_code,duz);
	if (br==2)
	{
		pom_short=(pom_short<<12)+isp;
		isp1=(pom_short>>8);
		fwrite(&isp1,sizeof(char),1,out);
		isp1=pom_short&((1<<8)-1);
		fwrite(&isp1,sizeof(char),1,out);
	}
	else
	{
		pom_short=isp>>4;
		unsigned char pom_zn=(unsigned char)pom_short;
		fwrite(&pom_zn,sizeof(char),1,out);
		pom_zn=(isp&15)<<4;
		fwrite(&pom_zn,sizeof(char),1,out);
	}
	clear_trie(&table);
}
////////////////
//REGION:DECOMPRESSION_TABLE
//////////////////

void init_decompression_table(decompression_table *dcmp)
{
	dcmp->duz=ASCII_LZW;
	int i;
	for (i=0;i<VELICINA_LZW;i++)
	{
		if (i<ASCII_LZW)
		{
			dcmp->table[i][0]=i;
			dcmp->l[i]=1;
		}
		else
		{
			dcmp->l[i]=0;
		}
	}
}
/*
*@brief: Funkcija utvrdjuje da li se kod nalazi u tabeli
*@param[in] dcmp dekompresiona tabela
*@param[in] ind kod za koji se utvrdjuje da li je u tabeli
*/
int contains_dcmp(decompression_table dcmp,unsigned short ind)
{
	return dcmp.l[ind]!=0;
}
/*
*@brief: Funkcija upisuje polje oznaceno odgovarajucim kljucem iz tabele u niz
*@param[out] pom_isp[] niz u koji se upisuje vrednost reci
*@param[in] dcmp dekompresiona tabela
*@param[in] ind kod reci
*@param[out] n DUZINA_LZW dobijene reci
*/
void translate(unsigned short pom_isp[],decompression_table dcmp,unsigned short ind,int *n)
{
	*n=dcmp.l[ind];
	int i;
	for (i=0;i<(*n);i++)
		pom_isp[i]=dcmp.table[ind][i];
}

void table_add(decompression_table *dcmp,unsigned short rec[],int n)
{
	unsigned short duz=dcmp->duz;

	if (duz+1>VELICINA_LZW)
		return;
	//izmena! dodat if
	if (n>=DUZINA_LZW)
		return;
	dodeli_razlcitih_duzina(dcmp->table[dcmp->duz],rec,(dcmp->l)+duz,n);
	dcmp->duz++;
}

void clear_dcmp(decompression_table *dcmp)
{
	dcmp->duz=257;
	int i;
	for (i=dcmp->duz;i<VELICINA_LZW;i++)
	{
		dcmp->l[i]=0;
	}
}
void ispisi(unsigned short pom_isp[],int n,FILE *out)
{
	int i;
	for (i=0;i<n;i++)
	{
		unsigned char znak=(unsigned char)pom_isp[i];
		fwrite(&znak,sizeof(char),1,out);
	}
}
unsigned short prva_cetiri(unsigned char c)
{
    return (c&(15<<4))>>4;
}
unsigned short poslednja_cetiri(unsigned char c)
{
    return (c&15);
}
void lzw_basic_decompression(FILE *in,FILE *out)
{
	unsigned char prazan;
	decompression_table dcmp;
	unsigned char character;
	int i,n=0,strclen=0,br=3;
	unsigned short string_code[VELICINA_LZW],zbir[VELICINA_LZW];
	init_decompression_table(&dcmp);
	unsigned short old_code,new_code=0,new1,new2;
	unsigned char buf1,buf2,buf3,buf;
	fread(&buf1,sizeof(char),1,in);
	fread(&buf2,sizeof(char),1,in); 
	old_code=(buf1<<4)+prva_cetiri(buf2);
	character=(unsigned char)old_code;
	unsigned short pom_isp[VELICINA_LZW];
	translate(pom_isp,dcmp,old_code,&n);
	ispisi(pom_isp,n,out);
	while (fread(&buf,sizeof(char),1,in)!=0)
	{
		switch (br)
		{
			case 1:
			{
				buf1=buf;
				new_code=buf;
				br=2;
				continue;
			}
			case 2:
			{
				buf2=buf;
				new_code=(new_code<<4)+prva_cetiri(buf2);
				br=3;
				break;
			}
			case 3:
			{
				buf3=buf;
				new_code=(poslednja_cetiri(buf2)<<8)+buf3;
				br=1;
				break;
			}
		}
		translate(pom_isp,dcmp,old_code,&n);
		dodeli_razlcitih_duzina(string_code,pom_isp,&strclen,n);
		saberi(zbir,string_code,character,strclen);
		if (!contains_dcmp(dcmp,new_code))
		{
			dodeli(string_code,zbir,&strclen);
		} 
		else
		{
			translate(pom_isp,dcmp,new_code,&n);
			dodeli_razlcitih_duzina(string_code,pom_isp,&strclen,n);
		}
		ispisi(string_code,strclen,out);
		character=string_code[0];
		translate(pom_isp,dcmp,old_code,&n);
		dodeli_razlcitih_duzina(string_code,pom_isp,&strclen,n);
		saberi(zbir,string_code,character,strclen);
		strclen++;
		table_add(&dcmp,zbir,strclen);
		old_code=new_code;
	}
	clear_dcmp(&dcmp);
}






