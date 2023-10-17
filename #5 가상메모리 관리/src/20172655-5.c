#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* const methods[4]={"OPT", "FIFO", "LRU", "Second-Chance"}; //페이지 교체 기법 스트링상수
int frame_cnt=0; //피지컬 프레임 개수(파일)
int *frame; //피지컬 프레임 상태 저장, 출력용
int ref_cnt=0; //페이지 레퍼런스 스트링 개수(파일)
int ref_arr[30]; //페이지 레퍼런스 스트링 저장 배열(파일)

void print_table(int); //테이블 출력용 함수
void print_frame_stat(void); //매 시점 프레임 상태 출력
void func_OPT(void);
void func_FIFO(void);
void func_LRU(void);
void func_Second_Chance(void);

int main(void) {
	char file_name[100]; //파일 명 입력 받는 배열
	FILE *fp; //파일포인터
	char temp[100], *tok; //파일 내용 임시 저장, 토크나이징에 사용

	printf("파일 이름 입력 : "); 
	scanf("%[^\n]", file_name);
	if(NULL==(fp=fopen(file_name, "r"))) {
		printf("Wrong file name.\n");
		exit(1);
	}
	printf("========================================\n");

	fgets(temp, 99, fp); 
	frame_cnt=atoi(temp); //피지컬 프레임 개수 

	fgets(temp, 99, fp); //페이지 레퍼런스 스트링
	temp[strlen(temp)-1]='\0';

	tok=strtok(temp,  " "); //페이지 레퍼런스 토큰 단위 분리, 저장
	while(tok!=NULL) {
		ref_arr[ref_cnt++]=atoi(tok);
		tok=strtok(NULL, " ");
	}
	
	for(int i=0; i<4; i++)
	{
		print_table(i);
		switch(i) {
			case 0://OPT
				func_OPT();
				break;
			case 1://FIFO
				func_FIFO();
				break;
			case 2://LRU
				func_LRU();
				break;
			case 3://Second_Chance
				func_Second_Chance();
				break;
		}
	}
	return 0;

}

void print_table(int i) {
	printf("Used method : %s\n", methods[i]);
	printf("page reference string : ");
	for(int i=0; i<ref_cnt; i++)
		printf("%d ", ref_arr[i]);
	printf("\n");
	printf("\n%13s   ", "frame");
	for(int i=1; i<=frame_cnt; i++)
		printf("%-8d", i);
	printf("page fault\n");
	printf("time\n");
	return;
}

void print_frame_stat(void) {
	for(int i=0; i<frame_cnt; i++) {
		if(frame[i]==-1)
			printf("%-8c", ' ');
		else 
			printf("%-8d", frame[i]);
	}
	return;
}

void func_OPT(void) {
	int hit=0, fault_cnt=0, stack_cnt=0, come_late=0;
	int *distance=(int*)calloc(frame_cnt, sizeof(int)); //현재 프레임 다음 호출까지의 거리
	frame=(int*)calloc(frame_cnt, sizeof(int)); //피지컬 프레임 상태 저장, 출력용
	for(int i=0; i<frame_cnt; i++) frame[i]=-1;
	for(int i=0; i<ref_cnt; i++) { // i : 현재 reference
		come_late=0;
		printf("%-16d", i+1);
		//계산
		for(int j=0; j<frame_cnt; j++) { // hit 여부 판단
			if(frame[j]==ref_arr[i]) { // hit
				hit=1;
			} 
		}
		if(hit==0) { //miss
			if(stack_cnt==frame_cnt) { // 남은 frame 없음
				for(int j=1; j<frame_cnt; j++) {
					if(distance[come_late]<distance[j])
						come_late=j;
				}
				frame[come_late]=ref_arr[i];							
			}
			else { // 빈 frame 있음
				for(int j=0; j<frame_cnt; j++) {
					if(frame[j]==-1) {
						frame[j]=ref_arr[i];
						stack_cnt++;
						break;
					}
				}
			}		
		}
		//distance 계산
		for(int j=0; j<frame_cnt; j++) {
			if(frame[j]!=-1) {
				distance[j]=100; //매우 먼 거리, 변동 X시 앞으로 등장하지 않는 페이지 의미
				for(int k=i+1; k<ref_cnt; k++) {
					if(frame[j]==ref_arr[k]) {
						distance[j]=k-(i+1);
						break;
					}
				}
			}
		}
			
		print_frame_stat();
		if(hit==0) { fault_cnt++; printf("F\n"); }
		else {hit=0; printf("\n"); }
	}
	printf("Number of page faults : %d times\n", fault_cnt);
	printf("========================================\n");
	free(distance);
	free(frame);
	return;
}

void func_FIFO(void) {
	int hit=0, fault_cnt=0, stack_cnt=0, mod=0;
	frame=(int*)calloc(frame_cnt, sizeof(int)); //피지컬 프레임 상태 저장, 출력용
	for(int i=0; i<frame_cnt; i++) frame[i]=-1; 
	for(int i=0; i<ref_cnt; i++) { // i : 현재 reference
		printf("%-16d", i+1);
		//계산
		for(int j=0; j<frame_cnt; j++) { // hit 여부 판단
			if(frame[j]==ref_arr[i]) {hit=1;} // hit
		}
		if(hit==0) { //miss
			if(stack_cnt==frame_cnt) { // 남은 frame 없음
				frame[mod]=ref_arr[i];
				mod=(mod+1)%frame_cnt;
			}
			else { // 빈 frame 있음
				for(int j=0; j<frame_cnt; j++) {
					if(frame[j]==-1) {
						frame[j]=ref_arr[i];
						stack_cnt++;
						break;
					}
				}
			}		
		}
		print_frame_stat();
		if(hit==0) { fault_cnt++; printf("F\n"); }
		else {hit=0; printf("\n"); }
	}
	printf("Number of page faults : %d times\n", fault_cnt);
	printf("========================================\n");
	free(frame);
	return;
}

void func_LRU(void) {
	int hit=0, fault_cnt=0, stack_cnt=0, index=0;
	int *counter=(int*)calloc(frame_cnt, sizeof(int)); //피지컬 프레임의 카운터, LRU 판단
	frame=(int*)calloc(frame_cnt, sizeof(int)); //피지컬 프레임 상태 저장, 출력용
	for(int i=0; i<frame_cnt; i++) frame[i]=-1;
	for(int i=0; i<ref_cnt; i++) { // i : 현재 reference
		printf("%-16d", i+1);
		//계산
		index=0;
		for(int j=0; j<frame_cnt; j++) { // hit 여부 판단
			if(frame[j]==ref_arr[i]) {
				counter[j]=0;
				hit=1;
				break;
			} // hit
		}
		if(hit==0) { //miss
			if(stack_cnt==frame_cnt) { // 남은 frame 없음
				for(int j=1; j<frame_cnt; j++) {
					if(counter[index]<counter[j])
						index=j; 
				}
				frame[index]=ref_arr[i];
				counter[index]=0;
			}
			else { // 빈 frame 있음
				for(int j=0; j<frame_cnt; j++) {
					if(frame[j]==-1) {
						frame[j]=ref_arr[i];
						stack_cnt++;
						break;
					}
				}
			}	
		}
		for(int j=0; j<frame_cnt; j++) { //페이지 들어있는 프레임의 카운터 증가 	
				if(frame[j]!=-1) counter[j]++;
		}
		print_frame_stat();

		if(hit==0) { fault_cnt++; printf("F\n"); }
		else {hit=0; printf("\n"); }
	}
	printf("Number of page faults : %d times\n", fault_cnt);
	printf("========================================\n");
	free(frame);
	free(counter);
	return;
}

void func_Second_Chance(void) {
	int hit=0, fault_cnt=0, stack_cnt=0, next_victim=0;
	int *label=(int*)calloc(frame_cnt, sizeof(int)); //히트 레이블
	frame=(int*)calloc(frame_cnt, sizeof(int)); //피지컬 프레임 상태 저장, 출력용
	for(int i=0; i<frame_cnt; i++) frame[i]=-1;
	for(int i=0; i<ref_cnt; i++) { // i : 현재 reference
		printf("%-16d", i+1);
		//계산
		for(int j=0; j<frame_cnt; j++) { // hit 여부 판단
			if(frame[j]==ref_arr[i]) { // hit
				hit=1;
				label[j]=1;
			} 
		}
		if(hit==0) { //miss
			if(stack_cnt==frame_cnt) { // 남은 frame 없음
				while(1) {
					if(label[next_victim]==1) {
						label[next_victim]=0;
						next_victim=(next_victim+1)%frame_cnt;
					}
					else {
						frame[next_victim]=ref_arr[i];
						next_victim=(next_victim+1)%frame_cnt;
						break;
					}
				}
			}
			else { // 빈 frame 있음
				for(int j=0; j<frame_cnt; j++) {
					if(frame[j]==-1) {
						frame[j]=ref_arr[i];
						label[j]=0;
						stack_cnt++;
						break;
					}
				}
			}		
		}
		print_frame_stat();
		if(hit==0) { fault_cnt++; printf("F\n"); }
		else {hit=0; printf("\n"); }
	}
	printf("Number of page faults : %d times\n", fault_cnt);
	printf("========================================\n");
	free(label);
	free(frame);
	return;
}
