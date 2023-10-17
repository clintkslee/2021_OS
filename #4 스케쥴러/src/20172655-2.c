/* 20172655 LEE KANG SAN */
/* CASE 2 : CFS scheduler - Priorty changed */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sched.h>
int main() {
	int sched = sched_getscheduler(getpid());
	switch (sched) {
		case 0: printf("scheduling policy : [%d] SCHED_OTHER\n", sched); break;
		case 1: printf("scheduling policy : [%d] SCHED_FIFO\n", sched); break;
		case 2: printf("scheduling policy : [%d] SCHED_RR\n", sched); break;
	}
	printf("**************************************************\n");
	pid_t pid_s[21], pid_e[21];
	for(int i=0; i<7; i++) { //GROUP1 : little task, nice = 15
		pid_s[i]=fork();
		if(pid_s[i]>0)
			printf("%d process bigins.\n", pid_s[i]);
		else if(pid_s[i]==0) { //task = calculate matrix 1 time
			setpriority(PRIO_PROCESS, getpid(), 15); 
			double A[500][500], B[500][500], C[500][500];	
			for(int i=0; i<500; i++) { //initialize matrix A, B
				for(int j=0; j<500; j++) {
					A[i][j]=(double)i+j;
					B[i][j]=1000.0-A[i][j];
				}
			}
			for(int i=0; i<500; i++) { //calculate 500*500*500 times
				for(int j=0; j<500; j++) {
					for(int k=0; k<500; k++) {
						C[i][j]+=A[i][k]*B[k][j];
					}
				}
			}
			printf("%d process ends.\n", getpid());
			exit(0);
		}
	}
	for(int i=7; i<14; i++) { //GROUP2, same task, nice = 0
		pid_s[i]=fork();
		if(pid_s[i]>0)
			printf("%d process bigins.\n", pid_s[i]);
		else if(pid_s[i]==0) { //children task = calculate matrix 3 times
			setpriority(PRIO_PROCESS, getpid(), 0); 
			double A[500][500], B[500][500], C[500][500];	
			for(int i=0; i<500; i++) { //initialize matrix A, B
				for(int j=0; j<500; j++) {
					A[i][j]=(double)i+j;
					B[i][j]=1000.0-A[i][j];
				}
			}
			for(int c=0; c<3; c++) {//calculate 3*500*500*500 times
				for(int i=0; i<500; i++) {
					for(int j=0; j<500; j++) {
						for(int k=0; k<500; k++) {
							C[i][j]+=A[i][k]*B[k][j];
						}
					}
				}
			}
			printf("%d process ends.\n", getpid());
			exit(0);
		}
	}
	for(int i=14; i<21; i++) { //GROUP3, more task, nice = -15
		pid_s[i]=fork();
		if(pid_s[i]>0)
			printf("%d process bigins.\n", pid_s[i]);
		else if(pid_s[i]==0) { //task = calculate matrix 5 times
			setpriority(PRIO_PROCESS, getpid(), -15); 
			double A[500][500], B[500][500], C[500][500];	
			for(int i=0; i<500; i++) { //initialize matrix A, B
				for(int j=0; j<500; j++) {
					A[i][j]=(double)i+j;
					B[i][j]=1000.0-A[i][j];
				}
			}
			for(int c=0; c<5; c++) {//calculate 5*500*500*500 times
				for(int i=0; i<500; i++) {
					for(int j=0; j<500; j++) {
						for(int k=0; k<500; k++) {
							C[i][j]+=A[i][k]*B[k][j];
						}
					}
				}
			}
			printf("%d process ends.\n", getpid());
			exit(0);
		}
	}

	for(int i=0; i<21; i++) 
		pid_e[i]=wait(NULL);
	printf("**************************************************\n");
	printf("BEGINNING order\n");
	for(int i=0; i<21; i++) {
		printf("[%d] ", pid_s[i]);
		if(i%7==6) printf("\n");
	}
	printf("ENDING order\n");
	for(int i=0; i<21; i++) {
		printf("[%d] ", pid_e[i]);
		if(i%7==6) printf("\n");
	}
	printf("**************************************************\n");
	printf("FINISHED\n");
	return 0;
}


