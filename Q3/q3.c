#include<pthread.h>
#include<semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<time.h>
sem_t ss[200005], ms[200005], coord;
pthread_mutex_t slock[200005],qu;
int t2,t1,t,a,e,k,c;
int sc[200005],stim[200005];
char sname[2000][100];
int sm[200005][2];
char typ[200005];
struct queue
{
	int q[200005];
	int ind;
} aq,eq;
void red () {
  printf("\033[1;31m");
}

void yellow() {
  printf("\033[1;33m");
}
void green() {
  printf("\033[1;32m");
}
void blue() {
  printf("\033[1;34m");
}


int insert(struct queue *qq, int x)
	{
		qq->ind=qq->ind+1;
		qq->q[qq->ind]=x;
		return qq->ind;
	}
int pop(struct queue *qq)
{
	if(qq->ind==-1)
		return -1;
	qq->ind=qq->ind-1;
	return qq->q[qq->ind+1];
}

void* stage(void* rd)
{
	int id=*(int*)rd;
	char ty = 'x';
	if(id<a)
		ty='a';
	else
		ty='e';
	while(1)
	{
		if(sc[id]==0)
			sem_wait(&ss[id]);
		int zz=rand()%(t2-t1) +t1;
		blue();
	    printf("%s playing %c started performance at stage %d of type %c for %d seconds \n", sname[sm[id][0]],typ[sm[id][0]],id, ty, zz);
		sleep(zz);
		int more2=0;
		pthread_mutex_lock(&slock[id]);
		if(sc[id]==2)
			more2=1;
		pthread_mutex_unlock(&slock[id]);
		if(more2)
		{
			sleep(2);
			sem_post(&ms[sm[id][0]]);
			sem_post(&ms[sm[id][1]]);
		}
		else
		{
			sem_post(&ms[sm[id][0]]);
		}
		pthread_mutex_lock(&slock[id]);
		sc[id]=0;
		pthread_mutex_unlock(&slock[id]);
		pthread_mutex_lock(&qu);
		if(id<a)
			insert(&aq, id);
		else
			insert(&eq, id);
		pthread_mutex_unlock(&qu);
	}
	return 0;
}
int getrand(int id)
{

	if(typ[id]=='v')
	{
		pthread_mutex_lock(&qu);
		int tmp= pop(&aq);
		pthread_mutex_unlock(&qu);
		return tmp;
	}
	else if(typ[id]=='b')
	{
		pthread_mutex_lock(&qu);
		int tmp= pop(&eq);
		pthread_mutex_unlock(&qu);
		return tmp;
	}
	pthread_mutex_lock(&qu);
	int x=0;
	if(clock()%2==0 && (x=pop(&aq))!=-1)
	{
	   pthread_mutex_unlock(&qu);
		return x;
	}
	else
	{
		x=pop(&eq);
	    pthread_mutex_unlock(&qu);
		return x;
	}
}
void *musician(void *rd)
{
	clock_t start=clock();
	int id=*(int*)rd;
	sleep(stim[id]);
	green();
	printf("%s playing %c has arrived at Srujana\n", sname[id], typ[id]);
	int pl=0;
	int st_id=0;
	int dbg=0;
	while((dbg=(clock()-start)/CLOCKS_PER_SEC)<t)
	{
		int x=getrand(id);
		if(x!=-1)
		{
			st_id=x;
			sm[x][0]=id;
			sc[x]++;
			sem_post(&ss[x]);
			pl=1;
			break;
		}
		if(typ[id]=='s')
		{
		x=rand()%(a+e);
		if(sc[x]==1)
		{
			pthread_mutex_lock(&slock[x]);
			if(sc[x]==1)
			{
				st_id=x;
				blue();
	            printf("%s singer joined musician %s at stage %d\n", sname[id],sname[sm[x][0]] ,x);
				sm[x][1]=id;
				sc[x]++;
				pl=1;
			}
			pthread_mutex_unlock(&slock[x]);
		}
		}
		if(pl)
			break;
	}
	if(pl)
	   sem_wait(&ms[id]);
	else
	{
		red();
	    printf("%s playing %c left Srujana due to impatience\n", sname[id], typ[id]);
		return 0;
	}
	char ty='x';
	if(st_id<a)
		ty='a';
	else
		ty='e';
	blue();
	printf("%s playing %c has finished his performance at stage %d of type %c.\n", sname[id], typ[id], st_id, ty);
	green();
	printf("%s playing %c is waiting for T-shirt.\n", sname[id], typ[id]);
		sem_wait(&coord);
		sleep(2);
		sem_post(&coord);
	green();
	printf("%s playing %c has collected his T-shirt.\n", sname[id], typ[id]);
	return 0;
}
pthread_t mus[200005];
int main()
{
	aq.ind=-1;
	eq.ind=-1;
	pthread_mutex_init(&qu, NULL);
	scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
	for(int i=0;i<k;i++)
	{
		scanf("%s %c %d", sname[i], &typ[i], &stim[i]);
	}
	sem_init(&coord,0,c);
	for(int i=0;i<a;i++)
	{
		int *tmp=(int*)malloc(sizeof(int));
		*tmp=i;
		insert(&aq,i);
		pthread_t xd;
		pthread_mutex_init(&slock[i], NULL);
	    sem_init(&ss[i], 0, 0);
		pthread_create(&xd,NULL,stage, tmp); 
	}
	for(int i=0;i<e;i++)
	{
		int *tmp=(int*)malloc(sizeof(int));
		*tmp=i+a;
		insert(&eq,i+a);
		pthread_mutex_init(&slock[i+a], NULL);
	    sem_init(&ss[i+a], 0, 0);
		pthread_t xd;
		pthread_create(&xd,NULL,stage, tmp); 
	}
	for(int i=0;i<k;i++)
	{
		int *tmp=(int*)malloc(sizeof(int));
		*tmp=i;
	    sem_init(&ms[k], 0, 0);
		pthread_create(&mus[i],NULL,musician, tmp); 
	}
	for(int i=0;i<k;i++)
		pthread_join(mus[i], NULL);
	red();
	printf("Finished\n");
	exit(0);
}
