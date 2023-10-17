#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

typedef struct{
	char PID[20];
	int UID;
	int PR;
	int NI;
	int VIRT;
	int RES;
	int SHR;
	char S;
	float CPU;
	float MEM;
	int TIME;
	char COMMAND[101];
}myproc; 


void printsystem(); //요약 출력
void printtask(); //task 출력
void printcpu(); //CPU usage 출력
void printmemory(); //memory usage 출력
void printdetail(); //process 상태 출력
int main()
{
	system("clear");
	while(1)
	{
		printsystem();
		printtask();
		printcpu();
		printmemory();
		printdetail();
		sleep(3); //top과 동일하게 매 3초마다 새로 출력
		system("clear");
	}
}
void printsystem() //uptime, loadavg
{//uptime 파일에서 시간을, loadavg파일에서 에서 1, 5, 15분간 load average 데이터 파싱 및 출력
	time_t t=time(NULL);
	struct tm tm= *localtime(&t);
	FILE *fp;
	float uptime, load1, load5, load15;
	
	printf("top - %02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);

	fp=fopen("/proc/uptime", "r");
	fscanf(fp, "%f", &uptime); 
	printf("up %02d:%02d, ", (int)uptime/60/60, (int)uptime/60%60);
	fclose(fp);
	
	printf("%2d user,  ", 1); ////

	fp=fopen("/proc/loadavg", "r");
	fscanf(fp, "%f", &load1); 
	fscanf(fp, "%f", &load5); 
	fscanf(fp, "%f", &load15); 
	printf("load average: %.2f, %.2f, %.2f", load1, load5, load15); 
	fclose(fp);
	
	printf("\n");
}
void printtask()
{ //동작중인 프로세스들의 state 종합 및 출력
	FILE* fp;
	char path[100]="/proc/";
	char status[8]="/status";
	char temp[101];
	char c;
	DIR* dp=opendir("/proc"); //proc 디렉토리 
	struct dirent* dentry; //proc 디렉토리용 엔트리
	int cnt=0; //proc 디렉토리 내 프로세스폴더 카운트	
	int R=0, S=0, T=0, Z=0;

	while((dentry=readdir(dp)) != NULL)
		if(atoi(dentry->d_name)!=0) //proc 디렉토리 내 프로세스 개수 카운트
			cnt++;
	rewinddir(dp);

	while((dentry=readdir(dp)) != NULL)
	{
		if(atoi(dentry->d_name)!=0)
		{
			strcat(path, dentry->d_name);
			strcat(path, status);
			fp=fopen(path, "r");
			fgets(temp, 100, fp);
			fgets(temp, 100, fp);
			fscanf(fp, "State:	%c", &c);
			switch (c)
			{
				case 'T':T++;break;
				case 'R':R++;break;
				case 'S':S++;break;
				case 'I':S++;break;
				case 'Z':Z++;break;
			}
			path[6]='\0';
			fclose(fp);
		}
	}
	printf("Tasks: %3d total, %3d running, %3d sleeping, %3d stopped, %3d zombie", cnt, R, S, T, Z);
	printf("\n");

	closedir(dp);
}
void printcpu()
{ //stat 파일을 참조하여 CPU usage 출력
	int us, sy, ni, id, wa, hi, si, st;
	float sum;
	FILE* fp=fopen("/proc/stat", "r");
	fscanf(fp, "cpu	%d", &us);
	fscanf(fp, " %d", &ni);
	fscanf(fp, " %d", &sy);
	fscanf(fp, " %d", &id);
	fscanf(fp, " %d", &wa);
	fscanf(fp, " %d", &hi);
	fscanf(fp, " %d", &si);
	fscanf(fp, " %d", &st);
	sum=0.0+us+sy+ni+id+wa+hi+si+st;

	////
	printf("%%Cpu(s): %4.1f us, %4.1f sy, %4.1f ni, %4.1f id, %4.1f wa, %4.1f hi, %4.1f si, %4.1f st", 
			us*100/sum, sy*100/sum, ni*100/sum, id*100/sum, wa*100/sum, hi*100/sum, si*100/sum, st*100/sum);
	fclose(fp);
	printf("\n");
}
void printmemory()
{ //meminfo 파일을 참조하여 memory usage 출력
	FILE* fp=fopen("/proc/meminfo", "r");
	char temp[100];
	int memtotal, memfree, memavailable, buffers, cached, swaptotal, swapfree, sreclaimable;
	fscanf(fp, "MemTotal: %d kB\n", &memtotal);
	fscanf(fp, "MemFree: %d kB\n", &memfree);
	fscanf(fp, "MemAvailable: %d kB\n", &memavailable);
	fscanf(fp, "Buffers: %d kB\n", &buffers);
	fscanf(fp, "Cached: %d kB\n", &cached);
	for(int i=0; i<9; i++)
		fgets(temp, 99, fp);
	fscanf(fp, "SwapTotal: %d kB\n", &swaptotal);
	fscanf(fp, "SwapFree: %d kB\n", &swapfree);
	for(int i=0; i<7; i++)
		fgets(temp, 99, fp);
	fscanf(fp, "SReclaimable: %d kB\n", &sreclaimable);
	printf("MiB Mem : %8.1f total, %8.1f free, %8.1f used, %8.1f buff/cache", ////
			memtotal/1000.0f, memfree/1000.0f, (memtotal-memfree-buffers-cached-sreclaimable)/1000.0f, (buffers+cached+sreclaimable)/1000.0f);
	printf("\n");
	printf("MiB Swap: %8.1f total, %8.1f free, %8.1f used, %8.1f avail Mem", ////
			swaptotal/1000.0f, swapfree/1000.0f, (swaptotal-swapfree)/1000.0f, memavailable/1000.0f);

	fclose(fp);
	printf("\n\n");
}
void printdetail()
{ //모든 프로세스의 자세한 정보 출력
	DIR* dp=opendir("/proc"); //proc 디렉토리 
	struct dirent* dentry; //proc 디렉토리용 엔트리
	int cnt=0; //proc 디렉토리 내 프로세스폴더 카운트
	int col=0;
	while((dentry=readdir(dp)) != NULL)
		if(atoi(dentry->d_name)!=0) //proc 디렉토리 내 프로세스 개수 카운트
			cnt++;

	myproc* mp=(myproc*)malloc(sizeof(myproc)*cnt);

	FILE* fp;
	char path[100]="/proc/";
	char temp[101];
	char* c;
	
	rewinddir(dp);
	while((dentry=readdir(dp)) != NULL) // 모든 프로세스에서 데이터 가져와 저장
	{
		if(col==cnt) break;
		if(atoi(dentry->d_name)!=0)
		{
			//char[] PID		
			strcpy(mp[col].PID, dentry->d_name);
			
			//char[] UID
			strcat(path, dentry->d_name);
			strcat(path, "/status");
			fp=fopen(path, "r");
			for(int i=0; i<8; i++)
				fgets(temp, 100, fp);
			fscanf(fp, "Uid: %d", &mp[col].UID);
			path[6]='\0';
			fclose(fp);
			
			//int PR //stat 18
			strcat(path, dentry->d_name);
			strcat(path, "/stat");
			fp=fopen(path, "r");
			fgets(temp, 100, fp);
			c=strtok(temp, " ");
			for(int i=0; i<17; i++)
				c=strtok(NULL, " ");
			mp[col].PR=atoi(c);
			path[6]='\0';
			fclose(fp);
			
			//int NI //stat 19
			strcat(path, dentry->d_name);
			strcat(path, "/stat");
			fp=fopen(path, "r");
			fgets(temp, 100, fp);
			c=strtok(temp, " ");
			for(int i=0; i<18; i++)
				c=strtok(NULL, " ");
			mp[col].NI=atoi(c);
			path[6]='\0';
			fclose(fp);
			
			//int VIRT
			
			//int RES
			
			//int SHR
			
			//char S
			strcat(path, dentry->d_name);
			strcat(path, "/status");
			fp=fopen(path, "r");
			fgets(temp, 100, fp);
			fgets(temp, 100, fp);
			fscanf(fp, "State:	%c", &(mp[col].S));
			path[6]='\0';
			fclose(fp);
			
			//float CPU
			
			//float MEM
			
			//int TIME //stat 14, 15, 16, 17
			strcat(path, dentry->d_name);
			strcat(path, "/stat");
			fp=fopen(path, "r");
			fgets(temp, 100, fp);
			c=strtok(temp, " ");
			for(int i=0; i<13; i++)
				c=strtok(NULL, " ");
			mp[col].TIME=atoi(c); //14
			c=strtok(NULL, " ");
			mp[col].TIME+=atoi(c); //15
			c=strtok(NULL, " ");
			mp[col].TIME+=atoi(c); //16
			c=strtok(NULL, " ");
			mp[col].TIME+=atoi(c); //17
			//mp[col].TIME/=100;
			path[6]='\0';
			fclose(fp);
			
			//char[] COMMAND
			strcat(path, dentry->d_name);
			strcat(path, "/comm");
			fp=fopen(path, "r");
			fgets(temp, 100, fp);
			strcpy(mp[col].COMMAND, temp);
			path[6]='\0';
			fclose(fp);

			col++;
		}
	}
	
	printf("    PID UID        PR  NI    VIRT    RES    SHR S  %%CPU  %%MEM     TIME+ COMMAND\n");    
	for(int i=0; i<cnt; i++)
		printf("%7s %-8d %4d %3d %7d %6d %6d %c %5.1f %5.1f  %02d:%02d.%02d %s", 
				mp[i].PID, mp[i].UID, mp[i].PR, mp[i].NI, mp[i].VIRT, mp[i].RES, mp[i].SHR, 
				mp[i].S, mp[i].CPU, mp[i].MEM, mp[i].TIME/6000, mp[i].TIME%6000/100, mp[i].TIME%60, mp[i].COMMAND);
	
	closedir(dp);
	free(mp);
}

