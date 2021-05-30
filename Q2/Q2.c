#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
struct comp_detail{
	int id;
	int batches;
	int batches_left;
	int batches_used;
	int prob;
	int cap;
};
struct zone_details{
	int id;
	int size;
	int prodfrom;
	int status;
	int phase;
};
struct student_detail{
	int id;
	int status;
	int zone;
	int try;
	int test_done;
	int result;
};
struct comp_detail company[1000];
struct zone_details zones[1000];
struct student_detail students[1000];
int drive,n,vacc,stu;
int o;
int flag;
int waiting_stu;
pthread_t company_tid[1000];
pthread_t students_tid[1000];
pthread_t zones_tid[1000];
pthread_mutex_t mutex;
pthread_mutex_t mutex2;
int min(int a , int b, int c){
	if(a<=b && a<=c){
		return a;
	}
	else if(b<=a && b<=c){
		return b;
	}
	else{
		return c;
	}
}
int do_test(int p){
	int k = rand()%100;
	if(k<p){
		return 1;
	}
	else{
		return 0;
	}
}
void* distribute_vacc(void* arg){
	struct comp_detail *args = arg;
	int p;
	//printf("Compnay %d has come\n",args->id );
	for(int i = 1;i<=vacc;i = i%vacc+1){
		if(drive == 0){
			//printf("Simulation Over\n");
			return NULL;
		}
		if(company[args->id].batches_left != 0){
			
			pthread_mutex_lock(&mutex);
			if(zones[i].status == 0){
				printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %d\n",args->id,i,args->prob );
				p  = args->cap;
				zones[i].status = 1;
				zones[i].prodfrom = args->id;
				zones[i].size = p;
				args->batches_left--;
				company[args->id].batches_left--;
				}
				pthread_mutex_unlock(&mutex);
		}
		if(company[args->id].batches_left == 0){
			break;
		}

	}
	printf("All the vaccines prepared by Pharmaceutical Company %d are distributed.Waitng to be used by the zones \n",args->id);
	while(company[args->id].batches_used!=0){
		if(drive == 0){
			return NULL;
			}
	}
	printf("All the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now.\n",args->id);
	return NULL;
}

void* make_vacc(void* arg){
	struct comp_detail *comp = arg;
	while(drive){
		int wait = 2 + (int)rand()%3;
		int batches_made = 1 + ((int)rand())%4;
		int p = 10 + ((int)rand())%10;
		int prob = comp->prob;
		printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %d percent\n",comp->id,batches_made,comp->prob);
		sleep(wait);
		printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %d percent\n", comp->id,batches_made,comp->prob);
		company[comp->id].batches = batches_made;
		company[comp->id].batches_left = batches_made;//left to be used
		company[comp->id].cap = p;
		company[comp->id].batches_used = batches_made;
		comp->batches = batches_made;
		comp->batches_left = batches_made;
		comp->batches_used = batches_made;
		comp-> cap = p;
		distribute_vacc(comp);

}
return NULL;
}

void* ini_stud(void* arg){
	struct student_detail *args = arg;
	int j = args->id;
	while(drive){
		if(students[j].try>=3){			
			if(o == 0){
				drive = 0;
			//	printf("Simulation Over.\n");
				return NULL;
			}
			return NULL;
			
		}
		else if (students[j].result == 0 && students[j].try<3 ){
		printf("Student %d has arrived for his round %d of Vaccination\n",args->id,students[args->id].try+1);
		
		printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n",args->id);
		pthread_mutex_lock(&mutex2);
		students[j].status = 1;
		waiting_stu++;
		pthread_mutex_unlock(&mutex2);
		while(students[j].status == 1){
if(drive == 0){
			return NULL;
			}
		}	
		while(students[j].test_done == 0){
			if(drive == 0){
			return NULL;
			}
		}
		students[j].test_done = 0;//test not done
		if(students[j].result == 1){			
			return NULL;
		}
	}
}

}

void* vaccinate(void* arg){
struct zone_details *args = arg;
int j = args->id;
int k;
	while(drive){
		flag = 0;
		while(zones[j].status == 0){
			if(drive == 0){
				//printf("Simulation Over\n");
				return NULL;
			}
		}
		int l = 0;
		pthread_mutex_lock(&mutex2);
		zones[j].phase = 1;
		int a = min(8,waiting_stu,zones[j].size);
		if((waiting_stu != 0 && zones[j].size != 0)){
			k = 1+rand()%a;
			//int i = 0;
			printf("Vaccination Zone %d is ready to vaccinate with %d slots\n",j,k );
			//int b = 1;
			l = 0;
			for(int b = 1;b<=stu;b++){
				if(students[b].status == 1 && l<k){
					printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n",b,j);
					students[b].status = 0;
					students[b].zone = j;
					l++;
					students[b].try +=1;
					flag =1;
				}
				if(l == k){
					break;
				}
				//printf("djdjd %d %d %d %d %d %d\n",b,k,waiting_stu,students[b].try,l,o);
			}
			waiting_stu = waiting_stu - k;
	}
		pthread_mutex_unlock(&mutex2);
		//b = 1;
		l = 0;
		if(flag)
		printf("Vaccination zone %d entering Vaccination Phase %d \n",j,k);
		flag = 0;
		for(int b = 1;b<=stu;b++){
			if(students[b].zone == j && l<k ){
				students[b].result = do_test(company[zones[j].prodfrom].prob);
				zones[j].size--;
				students[b].zone = 0;
				l++;
				students[b].test_done = 1;
				if(students[b].result == 1){
					o--;
					printf("Student %d has tested ‘positive’ for antibodies.\n",b );
					if(o == 0){
						drive = 0;
						//printf("Simulation Over.\n");
						return NULL;
					}
				}
				else{
					printf("Student %d has tested ‘negative’ for antibodies.\n",b );
					
					if(students[b].try <3){
					//printf("aaaa\n");
					}
					else{
						o--;
						if(o == 0){
							drive = 0;
							//printf("Simulation Over.\n");
							return NULL;
						}	
					}
				}
			}
		}
		if(zones[j].size == 0){
			company[zones[j].prodfrom].batches_used--;
			zones[j].status = 0;
			printf("Vaccination Zone %d has run out of vaccine\n",j);
		}
	}
}
int  main()
{	int i,x;
	/* code */
	drive = 1;
	printf("Enter the number of companies,vaccination zones and students\n");
	scanf("%d %d %d",&n,&vacc,&stu);
	o = stu;
	waiting_stu = 0;
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&mutex2,NULL);
	for(i = 1;i<=n;i++){
		scanf("%d",&x);
		company[i].id = i;
		company[i].prob = x;
		company[i].batches_left = 0;
		company[i].batches_used = 0;
	}
	if(n != 0 && vacc != 0 && stu!=0){
	for(i = 1;i<=vacc;i++){
		//pthread_mutex_init(&zone_m[i],NULL);
		zones[i].id = i;
		zones[i].status = 0;//0 means doesnt have any vaccine

	}
	for(i = 1;i<=stu;i++){
		students[i].id = i;
		students[i].try = 0;
		students[i].result = 0;
		students[i].zone = 0;
		students[i].status = 1;///ready for vacc;
	}
	srand(time(0));
	for(i = 1;i<= n;i++){
		struct comp_detail *a = malloc(sizeof(struct comp_detail));
		a -> id  = i;
		a->prob = company[i].prob;
		pthread_create(&company_tid[i],NULL,make_vacc,(void*)a);
	}
	for(i = 1;i<=stu;i++){
		struct student_detail *b = malloc(sizeof(struct student_detail));
		b -> id = i;
		b -> try = 0;
		pthread_create(&students_tid[i],NULL,ini_stud,(void*)b);
	}
	for(i = 1;i<=vacc;i++){
		struct zone_details *c = malloc(sizeof(struct zone_details));
		c -> id = i;
		pthread_create(&zones_tid[i],NULL,vaccinate,(void*)c);
	}
	for(i = 1;i<=n;i++){
		pthread_join(company_tid[i],NULL);
	}
	for(i = 1;i<=stu;i++){
		pthread_join(students_tid[i],NULL);
	}
	for(i = 1;i<=vacc;i++){
		pthread_join(zones_tid[i],NULL);
	}
	printf("Simulation Over.\n");
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);
}
else{
printf("Vaccinaton Drive not Possible\n");
	
}
return 0;
}