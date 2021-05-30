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
typedef long long int ll;
struct arg
{
	int l;
	int r;
	int* arr;
};
int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}
void selection_sort(int arr[],int n){
	int temp;
for(int i = 0;i<n-1;i++){
	int min = arr[i];
	for(int j = i+1;j<n;j++){
		if(min>arr[j]){
			min = arr[j];
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}

	}	
	}
}

void merge(int arr[],int l,int m,int r){
	int i,j,k;
	int n1 = m-l+1;
	int n2  = r - m;
	int L[n1],R[n2];
	for(i = 0;i<n1;i++){
		L[i] = arr[l+i];
	}
	for (j = 0; j < n2; j++) 
		R[j] = arr[m + 1+ j]; 
	i = 0;
	j = 0;
	k = l;
	while (i < n1 && j < n2) 
	{ 
		if (L[i] <= R[j]) 
		{ 
			arr[k] = L[i]; 
			i++; 
		} 
		else
		{ 
			arr[k] = R[j]; 
			j++; 
		} 
		k++; 
	} 

	 
	while (i < n1) 
	{ 
		arr[k] = L[i]; 
		i++; 
		k++; 
	} 



	while (j < n2) 
	{ 
		arr[k] = R[j]; 
		j++; 
		k++; 
	} 
	
}
void mergesort(int arr[],int l,int r){

if(l<r){
	int len = r-l+1;
	if(len<5){
		selection_sort(arr+l,len);
		return;
	}
	int m = (l+r)/2;
	mergesort(arr,l,m);
	mergesort(arr,m+1,r);
	merge(arr,l,m,r);
}
}

void normal_mergesort(int arr[],int n){
	mergesort(arr,0,n-1);
	 for(int i=0;i<n;i++){
                printf("%d ",arr[i]);
        }
        printf("\n");
}
void cmergesort(int arr[],int l,int r){
	int i,len,m;
	len = r-l+1;
	m = (l+r-1)/2;
	if(len<5){
		selection_sort(arr+l,len);
		return ;
	}
	int pid1,pid2;
	pid1 = fork();
	if(pid1<0){
	 perror("Left Child Proccess"); 
        exit(1);	
	}
	else if(pid1 == 0){
		cmergesort(arr,l,m);
		exit(0);
	}
	else{
		pid2 = fork();
		 if (pid2<0) 
        { 
            // Rchild proc not created 
            perror("Right Child Proc. not created\n"); 
            exit(1); 
        } 
        else if(pid2==0) 
        { 
            cmergesort(arr,m+1,r); 
            exit(0); 
        } 
        else{
        	int status;
			waitpid(pid1, &status, 0); 
		    waitpid(pid2, &status, 0); 
		    // Merge the sorted subarrays 
		    merge(arr, l, m, r);  
        }
        }	
}
void concurrent_mergesort(int arr[],int n){
	cmergesort(arr,0,n-1);
	for(int i = 0;i<n;i++){
		printf("%d ",arr[i]);
    }
        printf("\n");
}
void* threaded_mergeSort(void* a){
	struct arg *args = (struct arg*) a;

     int l = args->l;
     int r = args->r;
     int *crr = args->arr;
      
     if(l<r){
     	int len =r-l+1;
     	if(len<5){
     		selection_sort(crr+l,len);
     		return NULL;
     	}
     	int m = l+ (r-l)/2;
     	//sort left arr
     	struct arg leftarr;
     	leftarr.l = l;
     	leftarr.r = m;
     	leftarr.arr = crr;
     	pthread_t lefttid;
     	pthread_create(&lefttid,NULL,threaded_mergeSort,&leftarr);
     	//sortin right
     	struct arg rightarr;
        rightarr.l = m+1;
       rightarr.r = r;
       rightarr.arr = crr;
       pthread_t righttid;
       pthread_create(&righttid,NULL,threaded_mergeSort,&rightarr);
       //joining
       pthread_join(lefttid,NULL);
       pthread_join(righttid,NULL);

       merge(crr,l,m,r);

     }
     
}
void runsorts(){
	int n;
	scanf("%d",&n);
	int *arr = shareMem(sizeof(ll)*(n+1));
	int brr [n];
	int arr_thread[n];
	for(int i = 0;i<n;i++){
		scanf("%d",&arr[i]);
		brr[i] = arr[i];
		arr_thread[i] = arr[i];

	}

	struct timespec ts;

	
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
    // concurrent
    concurrent_mergesort(arr,n);

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double t1 = en-st;
    printf("time for concurrent mergesort is = %Lf\n", t1);
   
////////////////////////////////////////////////////////////////////////////////
    pthread_t tid;
     struct arg a;
     a.l = 0;
     a.r = n-1;
     a.arr = arr_thread;
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multithreaded mergesort
     pthread_create(&tid, NULL, threaded_mergeSort, &a);
     pthread_join(tid, NULL);

     printf("For threaded_mergeSort:\n");
     for(int i = 0;i<n;i++){
		printf("%d ",a.arr[i]);
    }
        printf("\n");
      clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     long double t3 = en-st;
     printf("time for threaded_mergeSort = %Lf\n", t3);
//////////////////////////////////////////////////////////////////////////
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
    //normal
    normal_mergesort(brr,n);

    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
   long double t2 = en-st;
    printf("time for normal_mergesort is  = %Lf\n", t2);

     


    printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n \t and [ %Lf ] times faster than threaded_mergesort\n\n ", t1/t2,t1/t3);
}
int main(){
	
	runsorts();
}