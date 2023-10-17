//20172655 LEE KANG SAN
//myps

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

typedef struct _proc{ //프로세스 정보를 담을 구조체
	char UID[20];
	char USER[20];
	char PID[20]; 
	int TTY; //
	int TIME;// jiffies time / 100 => seconds
	char CMD[50];
}MYPROC;


DIR* dp; //proc 디렉토리 
struct dirent* dentry; //proc 디렉토리용 엔트리
int cnt=0; //proc 디렉토리 내 프로세스폴더 카운트
int col=0; //프로세스 정보 저장된 배열의 인덱스로 사용

int cnt2=0; //passwd파일 내 계정 개수 카운트 

int main()
{
	FILE* fp; //to open proc file systems
	char path[100]="/proc/";
	const char status[8]="/status"; //status 파일 경로
	const char comm[6]="/comm"; //comm 파일 경로
	const char stat[6]="/stat"; //stat 파일 경로
	char temp[200];
	char* c; //

	FILE* fppw; //to open passwd
	char temp2[200]; //내용 파싱 위한 임시 배열
	char** pwarr; //etc/passwd 파일 내용 저장 위한 배열	
	fppw=fopen("/etc/passwd", "r");
	for(int i=0; fgets(temp2, 199, fppw)!=NULL; i++) //passwd내 계정 개수 파악 
		cnt2++; //count num of line in passwd file(=num of user)
	
	fseek(fppw, 0, SEEK_SET);
	
	pwarr=(char**)malloc(sizeof(char*)*cnt2);
	for(int i=0; i<cnt2; i++)
		pwarr[i]=(char*)malloc(sizeof(char)*101);

	for(int i=0; i<cnt2; i++) //passwd 내 모든 계정 정보 저장
		fgets(pwarr[i], 100, fppw);
	//get passwd file for UID


	dp=opendir("/proc"); 
	while((dentry=readdir(dp)) != NULL)
		if(atoi(dentry->d_name)!=0) //proc 디렉토리 내 숫자로만 이루어진 엔트리 카운트(프로세스 개수)
			cnt++;
	rewinddir(dp);
	MYPROC* p=(MYPROC*)malloc(sizeof(MYPROC)*cnt); //p[] : for saving process data
	
	while((dentry=readdir(dp)) != NULL)
	{
		if(col==cnt) break; //cnt : proc 디렉토리에서 파악한 프로세스 개수
		if(atoi(dentry->d_name)!=0) //number name dir == process
		{
		
			//open status : PID, UID
			strcat(path, dentry->d_name); 
			strcat(path, status);
			fp=fopen(path, "r");
			for(int i=1; fgets(temp, 199, fp)!=NULL; i++)
			{
				if(i==6) //status 파일의 6번째 정보가 Pid
				{
					c=strtok(temp, "Pid:	 "); 
					strcpy(p[col].PID, c);
					c=strchr(p[col].PID, '\n');
					c[0]='\0';
				}
				else if(i==9) //status 파일의 9번째 정보가 Uid
				{
					c=strtok(temp, "Uid:	 ");
					strcpy(p[col].UID, c);
				}
			}
			fclose(fp);
			path[6]='\0';
			
			//open comm : CMD
			strcat(path, dentry->d_name); 
			strcat(path, comm);
			fp=fopen(path, "r");
			if(fgets(temp, 199, fp)!=NULL)
			{
				strcpy(p[col].CMD, temp);
				c=strchr(p[col].CMD, '\n');
				c[0]='\0';
			}
			fclose(fp);
			path[6]='\0';
			
			
			//open stat : TTY, TIME
			p[col].TIME=0;
			strcat(path, dentry->d_name); 
			strcat(path, stat);
			fp=fopen(path, "r");
			fgets(temp, 199, fp);

			c=strtok(temp, " ");
			for(int i=1; i<18; i++)
			{
				if(i==7) //stat 파일의 7번째 필드가 tty_nr(TTY정보)
					p[col].TTY=atoi(c);
				
				else if(14<=i&&i<=17) //stat 파일의 14~17번째 정보가 cpu time 관련 정보(jiffies)
					p[col].TIME+=atoi(c);
				
				c=strtok(NULL, " ");
			}
			p[col].TIME/=100; //jiffies를 시분초 단위로 바꾸기 위한 적절한 연산
			fclose(fp);
			
			path[6]='\0';
			
			col++; //col번째 프로세스 정보 p[col]에 저장 완료 후 반복
		}
	}

	char* d; //UID를 passwd 파일의 정보와 매칭하여 User 정보 파악위해 사용
	for(int i=0; i<cnt; i++) //set USER by UID 
	{ 	 //passwd의 첫번째 필드 : user, 세번째 필드 : UID
		for(int j=0; j<cnt2; j++)
		{
			strcpy(temp2, pwarr[j]); 	
			c=strtok(temp2, ":");
			d=c;			//d = USER
			c=strtok(NULL, ":");
			c=strtok(NULL, ":");   //c = UID
			if(strcmp(c, p[i].UID)==0)
			{
				strcpy(p[i].USER, d);
				break;
			}
		}
	}
	printf("USER                PID TTY          TIME CMD\n"); //출력
	for(int i=0; i<cnt; i++)
	{
		printf("%-18s%5s %-9d%02d:%02d:%02d %-20s\n", p[i].USER, p[i].PID, p[i].TTY, p[i].TIME/3600, p[i].TIME/60, p[i].TIME%60, p[i].CMD);
	}
}
