#include <stdio.h>
#include <stdlib.h>

#define  ASCII_SH 256

typedef struct shuffman
{
	unsigned char znak;
	struct shuffman *l,*d;
	int v,list;//nadam se da je milijardu znakova dovoljno, mozda treba menjati  
}StaticHuffmanTree;
typedef struct priority_queue
{
	StaticHuffmanTree *heap[ASCII_SH+1];//1-based
	int duz;
}PriorityQueue;
typedef struct sifra
{
	unsigned short v;
	int br_bitova;//ovo je zbog vodecih nula
}Sifra;
typedef struct sh_queue
{
	StaticHuffmanTree *v;
	Sifra code;
	struct  sh_queue *sl;
}SH_Queue;
typedef struct par
{
	unsigned char zn;
	int p;	
}Par;

//////queue
int sh_queue_empty(SH_Queue *front)
{
	return (front==NULL);
}
void sh_queue_push(SH_Queue **front, SH_Queue **rear,StaticHuffmanTree *cvor,Sifra code)
{
	SH_Queue *pom=malloc(sizeof(SH_Queue));
	pom->v=cvor;
	pom->code=code;
	pom->sl=NULL;
	if (*rear)
	{
		(*rear)->sl=pom;
	}
	(*rear)=pom;
	if (!(*front))
	{
		*front=pom;
	}
}
void sh_queue_pop(SH_Queue **front, SH_Queue **rear)
{
	SH_Queue *novi=(*front)->sl,*stari=*front;
	*front=novi;
	free(stari);
	if (!front)
	{
		*rear=NULL;
	}
}
///SHT/////////
int leaf(StaticHuffmanTree *cvor)
{
	//return (cvor->d==NULL && cvor->l==NULL);
	return cvor->list;
}
StaticHuffmanTree *get_node(int p[],int ind)
{
	StaticHuffmanTree *novi=malloc(sizeof(StaticHuffmanTree));
	novi->znak=ind;
	novi->v=p[ind];
	novi->l=novi->d=NULL;
	novi->list=1;
	return novi;
}
StaticHuffmanTree *merge(StaticHuffmanTree **x,StaticHuffmanTree **y,int v)
{
	StaticHuffmanTree *z=malloc(sizeof(StaticHuffmanTree));
	z->v=v;
	z->l=*x;
	z->d=*y;
	z->list=0;
	return z;
}
Sifra sh_output(StaticHuffmanTree *koren,unsigned char znak)
{
	short stp=1,code;
	SH_Queue *front,*rear;
	front=NULL;
	rear=NULL;
	Sifra s1={0,0},rez;
	sh_queue_push(&front,&rear,koren,s1);
	SH_Queue *tek;
	while (!sh_queue_empty(front))
	{
		tek=front;
		if (leaf(tek->v) && tek->v->znak==znak)
		{
			rez=tek->code;
			break;
		}
		StaticHuffmanTree *pom=tek->v;
		Sifra pomsif;
		if (pom->l)
		{
			pomsif.v=tek->code.v*2+0;
			pomsif.br_bitova=tek->code.br_bitova+1;
			sh_queue_push(&front,&rear,pom->l,pomsif);
		}
		if (pom->d)
		{
			pomsif.v=tek->code.v*2+1;
			pomsif.br_bitova=tek->code.br_bitova+1;
			sh_queue_push(&front,&rear,pom->d,pomsif);
		}
		sh_queue_pop(&front,&rear);
	}
	while (!sh_queue_empty(front))
	{
		sh_queue_pop(&front,&rear);
	}
	return rez;
}
////PQ////////////
void pq_init(PriorityQueue *pq)
{
	pq->duz=0;
}
int pq_empty(PriorityQueue pq)
{
	return(pq.duz==0);
}
StaticHuffmanTree *pq_front(PriorityQueue pq)
{
	return pq.heap[1];
}
void pq_push(PriorityQueue *pq,StaticHuffmanTree *novi)
{
	int duz=pq->duz+1,ind=duz,pret;
	pq->heap[duz]=novi;
	while (ind!=1 && pq->heap[ind]->v<pq->heap[ind/2]->v)
	{
		StaticHuffmanTree *pom=pq->heap[ind];
		int pret=ind/2;
		pq->heap[ind]=pq->heap[pret];
		pq->heap[pret]=pom;
		ind=pret;
	}
	pq->duz++;
}
void pq_pop(PriorityQueue *pq)
{
	int kor=1,duz=pq->duz,ind=1;
	StaticHuffmanTree *pom=pq->heap[kor];
	pq->heap[kor]=pq->heap[duz];
	pq->heap[duz]=pom;
	while (ind*2<duz)
	{
		int l=2*ind,d=2*ind+1,manji;
		manji=l;
		if (d<duz && pq->heap[d]->v<pq->heap[l]->v)
		{
			manji=d;
		}
		pom=pq->heap[ind];
		if (pom->v<pq->heap[manji]->v)
		{
			break;
		}
		pq->heap[ind]=pq->heap[manji];
		pq->heap[manji]=pom;
		ind=manji;
	}
	pq->duz--;
} 

StaticHuffmanTree *build_SHTree(int p[])
{
	int i,num=0,v;
	PriorityQueue pq;
	pq_init(&pq);
	for (i=0;i<ASCII_SH;i++)
	{
		if (p[i]==0)
			continue;
		StaticHuffmanTree *novi=get_node(p,i);
		pq_push(&pq,novi);
	}
	StaticHuffmanTree *x,*y,*z;
	while (pq.duz>1)
	{
		x=pq_front(pq);
		v=x->v;
		pq_pop(&pq);
		y=pq_front(pq);
		v+=y->v;
		z=merge(&x,&y,v);
		pq_pop(&pq);
		pq_push(&pq,z);
	}
	return z;
}
void ispisi_sifru(Sifra s)
{
	short code=s.v;
	int brb=s.br_bitova-1;
	for (brb;brb>=0;brb--)
		printf("%d",((code&(1<<brb)))!=0);
	printf("\n");
}
void prebroj_verovatnoce_pojavljivanja(FILE *in,int p[])
{
	int i;
	unsigned char c;
	while (fread(&c,1,1,in)!=0)
	{
		p[c]++;
	}
	rewind(in);
}
void SH_initp(int p[])
{
	int i;
	for (i=0;i<ASCII_SH;i++)
	{
		p[i]=0;
	}
}

void ispisi_indeks_datoteke(FILE *out)
{
	fputc(1,out);
}
unsigned char prvih_k(Sifra s,int k)
{
	int n=s.br_bitova,v=s.v,pom=n-k;
	short mask=(1<<k)-1;
	return (unsigned char)((v&(mask<<pom))>>pom);
}
unsigned short odseci_k(Sifra s,int k)
{
	return s.v-(prvih_k(s,k)<<(s.br_bitova-k));
}
void na_kraj_datoteke(unsigned char vis,FILE *out)
{
	fwrite(&vis,1,1,out);
}
void ispisi_verovatnoce(int p[],FILE *out)
{
	int i;
	for (i=0;i<ASCII_SH;i++)
	{
		fwrite(&p[i],sizeof(int),1,out);
	}
}
void SH_basic_compression(FILE *in,FILE *out)
{
	int i,k;
	int p[ASCII_SH];
	SH_initp(p);
	prebroj_verovatnoce_pojavljivanja(in,p);
	StaticHuffmanTree *koren=build_SHTree(p);
	unsigned char znak,isp,vis;
	Sifra tr,ost={0,0};
	ispisi_indeks_datoteke(out);
	ispisi_verovatnoce(p,out);
	while (fread(&znak,1,1,in)>0)
	{
		tr=sh_output(koren,znak);
		/*ispisi_sifru(ost);
		ispisi_sifru(tr);
		printf("\n\n");*/
		if (tr.br_bitova+ost.br_bitova<=8)
		{
			ost.v=(ost.v<<tr.br_bitova)+tr.v;
			ost.br_bitova+=tr.br_bitova;
			if (ost.br_bitova==8)
			{
				isp=(unsigned char)ost.v;
				fwrite(&isp,1,1,out);
				//printf("%d\n",isp);
				ost.v=0;
				ost.br_bitova=0;
			}
		}
		else
		{
			k=(8-ost.br_bitova);
			isp=(unsigned char)((ost.v<<k)+prvih_k(tr,k));
			fwrite(&isp,1,1,out);
			//printf("%d\n",isp);
			tr.v=odseci_k(tr,k);
			tr.br_bitova-=k;
			while (tr.br_bitova>=8)
			{
				isp=(unsigned char)(prvih_k(tr,8));
				fwrite(&isp,1,1,out);
				//printf("%d\n",isp);
				tr.v=odseci_k(tr,8);
				tr.br_bitova-=8;
			}
			ost=tr;
		}
	}
	vis=8-ost.br_bitova;
	isp=(unsigned char)(ost.v<<vis);
	fwrite(&isp,1,1,out);
	//printf("%d\n",isp);
	tr.v=odseci_k(tr,k);
	na_kraj_datoteke(vis,out);
	//printf("%d\n",vis);
}
void ucitaj_indeks(FILE *in)
{
	unsigned char c;
	fread(&c,1,1,in);
}
void ucitaj_verovatnoce(FILE *in,int p[])
{
	int i;
	for (i=0;i<ASCII_SH;i++)
	{
		fread(&p[i],sizeof(int),1,in);
	}
}
unsigned char poslednji_bajt(FILE *in)
{
	fseek(in,-1,SEEK_END);
	unsigned char posl;
	fread(&posl,1,1,in);
	rewind(in);
	return posl;
}
int posl_smislen_bit(FILE * in,unsigned char vis)
{
	int uk=0,bajtovi=0;
	Sifra s;
	unsigned char zn;
	fseek(in,1+ASCII_SH*sizeof(int),SEEK_SET);//indeks algoritma + verovatnoce
	while (fread(&zn,1,1,in)>0)
	{
		bajtovi++;
	}
	bajtovi--;//footer
	uk=8*bajtovi-vis;
	rewind(in);
	return uk;
}
void SH_basic_decompression(FILE *in,FILE *out)
{
	int p[ASCII_SH],ind=0,pom=8,i,smer;
	unsigned char vis=poslednji_bajt(in),znak,pocetak=1,isp;
	int kraj=posl_smislen_bit(in,vis);
	ucitaj_indeks(in);
	ucitaj_verovatnoce(in,p);
	StaticHuffmanTree *koren=build_SHTree(p),*tr=koren;
	//printf("%d %d\n",vis,kraj);
	while (ind<kraj)
	{
		fread(&znak,1,1,in);
		for (i=7;i>=0;i--)
		{
			smer=((znak&(1<<i))!=0);
			if (ind>=kraj)
			{
				break;
			}
			if (smer==0)
			{
				tr=tr->l;
			}
			else
			{
				tr=tr->d;
			}
			if (leaf(tr))
			{
				isp=tr->znak;
				fwrite(&isp,1,1,out);
				tr=koren;
			}
			ind++;
		}
	}
}


