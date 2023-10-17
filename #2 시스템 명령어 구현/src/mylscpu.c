//20172655 LEE KANG SAN
//mylscpu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct{ //정보를 담을 구조체
	char* vendor_id;  //0
	char* model_name; //1
	int num_of_core;  //
	char* cpu_speed;  //2
	int cache_l1d;  //3
	int cache_l1i;  //4
	int cache_l2;   //5
	int cache_l3;
}mycpu; //cpu 정보 담을 구조체 변수

typedef struct _cache{ //캐시 관련 정보를 담을 구조체
	char level[20]; //1, 2, 3
	char size[20]; 
	char type[20]; //Data, Instruction, Unified
	char ways[20];
}CACHE;

CACHE ca[4]; //캐시 정보 담을 구조체 변수


void setmycpu(char**); //proc/cpuinfo로부터 받아온 정보를 구조체에 저장한다
void setmember(int, char* ,char**); //실제 저장 연산이 이루어지는 함수
void mylscpu(); //내용 출력
void mylscpucache(); //--cache 내용 출력

int cnt=0; // num of line
int main(int argc, char** argv)
{
	const char* path="/proc/cpuinfo";
	FILE* fp;
	char temp[1000];
	char** cpuinfo; //cpuinfo 파일로부터 파싱한 데이터 전부 저장될 배열
	if((fp=fopen(path, "r"))==NULL)
		exit(1);
	while(!feof(fp)) //count num of line
	{
		cnt++;
		fgets(temp, 1000, fp);
	}
	fseek(fp, 0, SEEK_SET);
	cpuinfo=(char**)malloc(sizeof(char*)*cnt);
	for(int i=0; i<cnt; i++)
		cpuinfo[i]=(char*)malloc(sizeof(char)*1000);

	for(int i=0; i<cnt; i++) //get cpuinfo
		fgets(cpuinfo[i], 1000, fp); //cpuinfo 데이터
	
	setmycpu(cpuinfo); //cpuinfo 배열 정보를 바탕으로 mycpu 구조체에 데이터 저장

	if(argc==1) //옵션 없이 호출 시
		mylscpu();
	else if(argc==2 && strcmp(argv[1], "--cache")==0) //--cache 옵션으로 호출 시
		mylscpucache();

	fclose(fp);
	return 0;
}
		
void setmycpu(char** cpuinfo) //proc/cpuinfo로부터 받아온 정보를 구조체에 저장한다
{
	for(int j=0, i=0; i<cnt; i++) //cpuinfo는 코어 단위로 정보를 보여주므로 processor 개수를 세어 코어 수 계산
	{
		if(strstr(cpuinfo[i], "processor")!=NULL) j++;
		mycpu.num_of_core=j;
	}

	//get info from proc/cpuinfo
	setmember(0, "vendor_id", cpuinfo);
	setmember(1, "model name", cpuinfo);
	setmember(2, "cpu MHz", cpuinfo);

	//get info of caches
	char p[4][100]={"/sys/devices/system/cpu/cpu0/cache/index0/",
		"/sys/devices/system/cpu/cpu0/cache/index1/",
		"/sys/devices/system/cpu/cpu0/cache/index2/",
		"/sys/devices/system/cpu/cpu0/cache/index3/"};
	char* s[4]={"level", "size", "type", "ways_of_associativity"};
	FILE* fp;
	for(int i=0; i<4; i++) //하나의 코어에서 L1d, L1i, L2, L3의 정보를 각각 추출
	{
		for(int j=0; j<4; j++)
		{
			strcat(p[j], s[i]);
			fp=fopen(p[j], "r");
			if(i==0) {
				fgets(ca[j].level, 20, fp);
				for(int a=0; a<20; a++)
					if(ca[j].level[a]=='\n') {
						ca[j].level[a]='\0';
						break;
					}
			}
			else if(i==1) {
				fgets(ca[j].size, 20, fp);
				for(int a=0; a<20; a++)
					if(ca[j].size[a]=='\n') {
						ca[j].size[a]='\0';
						break;
					}
			}
			else if(i==2) {
				fgets(ca[j].type, 20, fp);
				for(int a=0; a<20; a++)
					if(ca[j].type[a]=='\n') {
						ca[j].type[a]='\0';
						break;
					}
			}
			else if(i==3) {
				fgets(ca[j].ways, 20, fp);
				for(int a=0; a<20; a++)
					if(ca[j].ways[a]=='\n') {
						ca[j].ways[a]='\0';
						break;
					}
			}
			fclose(fp);
			p[j][42]='\0';	
		}
	}
	//set caches
	for(int i=0; i<4; i++) //추출된 캐시 정보를 바탕으로 mycpu 구조체 내의 캐시 크기 정보를 저장
	{
		if(strstr(ca[i].level, "1")!=NULL)
		{
			if(strstr(ca[i].type, "Data")!=NULL)
				mycpu.cache_l1d=atoi(ca[i].size)*mycpu.num_of_core; //L1, Data
			else
				mycpu.cache_l1i=atoi(ca[i].size)*mycpu.num_of_core; //L1, Instruction

		}
		else if(strstr(ca[i].level, "2")!=NULL)
				mycpu.cache_l2=atoi(ca[i].size)*mycpu.num_of_core; //L2
		else if(strstr(ca[i].level, "3")!=NULL)
				mycpu.cache_l3=atoi(ca[i].size)*mycpu.num_of_core; //L3
	}
}

void setmember(int mem, char* str,char** cpuinfo) //mem은 구조체 내의 멤버 접근(switch)위해 사용
{						    	
	char* ptr;
	for(int i=0; i<cnt; i++)
	{
		if((ptr=strstr(cpuinfo[i], str))!=NULL) 
		{
			ptr=strstr(cpuinfo[i], ":")+2;
			switch (mem)
			{
				case 0:mycpu.vendor_id=ptr; break;
				case 1:mycpu.model_name=ptr; break;
				case 2:mycpu.cpu_speed=ptr; break;
			}
			break;
		}
	}			
}

void mylscpu() //cpu 정보 출력
{
	printf("%-20s : %s", "Vendor ID", mycpu.vendor_id);
	printf("%-20s : %s", "Model name", mycpu.model_name);
	printf("%-20s : %d\n", "CPU(s)", mycpu.num_of_core);
	printf("%-20s : %s", "CPU MHz", mycpu.cpu_speed);
	printf("%-20s : %d KiB\n", "L1d cache", mycpu.cache_l1d);
	printf("%-20s : %d KiB\n", "L1i cache", mycpu.cache_l1i);
	printf("%-20s : %d KiB\n", "L2 cache", mycpu.cache_l2);
	printf("%-20s : %d MiB\n", "L3 cache", mycpu.cache_l3/1000);
}

void mylscpucache() //cache 정보 출력
{
	printf("NAME ONE-SIZE ALL-SIZE WAYS TYPE        LEVEL\n");
	
	for(int i=0; i<4; i++){
		if(strstr(ca[i].level, "1")!=NULL){
			if(strstr(ca[i].type, "Data")!=NULL)
				printf("L1d %9s%8dK%5s %-12s%5s\n", ca[i].size, mycpu.cache_l1d, ca[i].ways, ca[i].type, ca[i].level);
	}}
	for(int i=0; i<4; i++){
		if(strstr(ca[i].level, "1")!=NULL){
			if(strstr(ca[i].type, "Instruction")!=NULL)
				printf("L1i %9s%8dK%5s %-12s%5s\n", ca[i].size, mycpu.cache_l1i, ca[i].ways, ca[i].type, ca[i].level);
	}}
	for(int i=0; i<4; i++)
		if(strstr(ca[i].level, "2")!=NULL)
				printf("L2  %9s%8dK%5s %-12s%5s\n", ca[i].size, mycpu.cache_l2, ca[i].ways, ca[i].type, ca[i].level);
	for(int i=0; i<4; i++)
		if(strstr(ca[i].level, "3")!=NULL)
				printf("L3  %9s%8dK%5s %-12s%5s\n", ca[i].size, mycpu.cache_l3, ca[i].ways, ca[i].type, ca[i].level);
}
