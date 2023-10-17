#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define true 1


//function of threads
void *path_go1(void *null);
void *path_go2(void *null);
void *path_go3(void *null);
void *path_go4(void *null);

//tick
int tick=0;

//mutex, condition var
pthread_mutex_t mutex;
pthread_cond_t cond0, cond1, cond2, cond3, cond4;
pthread_t tid[5];

//critical section
int passing_queue[100]; // FIFO queue
int pass_front=0, pass_end=0, passed=0; //pop from front, push at end
int passed_cnt[5]; //passed cnt poped from passing queue, [0] is amount

int waiting_queue[100]; //bag
int wait_front=0, wait_end=0; //push at end, front<end
int wait_cnt[5]; //wait cnt in waiting queue, [0] is amount

int main() {
   //INIT MUTEX, INIT COND VAR, CREATE THREADS,
   pthread_mutex_init(&mutex, NULL); 
   pthread_cond_init(&cond0, NULL);   
   pthread_cond_init(&cond1, NULL);
   pthread_cond_init(&cond2, NULL);
   pthread_cond_init(&cond3, NULL);
   pthread_cond_init(&cond4, NULL);
   pthread_create(&tid[1], NULL, path_go1, NULL);
   pthread_create(&tid[2], NULL, path_go2, NULL);
   pthread_create(&tid[3], NULL, path_go3, NULL);
   pthread_create(&tid[4], NULL, path_go4, NULL);
////////////////////////////////////////////////////////////////////////////////////      
   //CREATE STARTING POINT LIST
   int num_of_vehicle,   //user input 
   *start_list,      //array, 1~4 random value     
   index=0;      //cursor for start_list[]
   srand(time(NULL));
   printf("Total number of vehicles : ");
   if(1!=scanf("%d", &num_of_vehicle) || 
      !(10<=num_of_vehicle && num_of_vehicle<=15)) {
      printf("Out of Range.\n");
      exit(1);
   }
   start_list=(int*)malloc(sizeof(int)*num_of_vehicle);
   printf("Start point : ");
   for(int i=0; i<num_of_vehicle; i++) {
      start_list[i]=1+rand()%4;
      printf("%d ", start_list[i]);
   }
   printf("\n");
/////////////////////////////////////////////////////////////////////////////////////////
   //main thread -> insert car into waiting queue, select one
   int choice;
   while(true) {
      pthread_mutex_lock(&mutex);
      if(passed_cnt[0]==num_of_vehicle) break; //모든 차량 pass 완료
      
      choice=0;
      tick++;

      printf("tick : %d\n", tick);
      printf("===============================\n");
///////////////////////////////////////////////////////////////////////////////////////////
      if(index<num_of_vehicle) { //매 틱마다 대기큐에 하나씩 삽입
         waiting_queue[wait_end]=start_list[index];
         wait_cnt[0]++; //대기큐 내 총 개수
         wait_cnt[start_list[index]]++; //특정도로 위 대기수
         wait_end++; //대기큐 삽입커서이동
         index++; //start_list 커서이동
      }
///////////////////////////////////////////////////////////////////////////////////////////

      switch(passing_queue[pass_front]) {
         case 1:
            pthread_cond_signal(&cond1);
            pthread_cond_wait(&cond0, &mutex);
            break;
         case 2:
            pthread_cond_signal(&cond2);
            pthread_cond_wait(&cond0, &mutex);
            break;
         case 3:
            pthread_cond_signal(&cond3);
            pthread_cond_wait(&cond0, &mutex);
            break;
         case 4:
            pthread_cond_signal(&cond4);
            pthread_cond_wait(&cond0, &mutex);
            break;
      }
      if(passing_queue[pass_front]==0) {//도로 위에 차 없는 경우      
    if(wait_end-wait_front!=0)
            choice=waiting_queue[wait_front+rand()%(wait_end-wait_front)];//대기큐 내에서 아무 원소나 선택, 패싱큐에 넣는다
         switch(choice) {
            case 1: //thread1 출발
               pthread_cond_signal(&cond1);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 2: //thread2 출발
               pthread_cond_signal(&cond2);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 3: //thread3 출발
               pthread_cond_signal(&cond3);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 4: //thread4 출발
               pthread_cond_signal(&cond4);
               pthread_cond_wait(&cond0, &mutex);
               break;
         }
         
      }   
          
      else { //도로 위에 차가 있는 경우 - 대응하는 길에서만 출발 가능
         if(passing_queue[pass_front]==1) { //1번도로 주행 중
            if(wait_cnt[3]>0) choice=3; //3번만 출발 가능
            else choice=0; //3번 대기차량 없으면 0
         }
         else if(passing_queue[pass_front]==2) {
            if(wait_cnt[4]>0) choice=4;
            else choice=0;
         }
         else if(passing_queue[pass_front]==3) {
            if(wait_cnt[1]>0) choice=1;
            else choice=0;
         }
         else if(passing_queue[pass_front]==4) {
            if(wait_cnt[2]>0) choice=2;
            else choice=0;
         }
         switch(choice) {
            case 1: pthread_cond_signal(&cond1);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 2: pthread_cond_signal(&cond2);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 3: pthread_cond_signal(&cond3);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 4: pthread_cond_signal(&cond4);
               pthread_cond_wait(&cond0, &mutex);
               break;
            case 0: break;
         }
      }      
      //현재 달리는 중인 차 빼기(목적지도착)      
      
      printf("Waiting Vehicle\n");
      printf("Car "); 
      for(int i=wait_front; i<wait_end; i++)
         printf("%d ", waiting_queue[i]);
      printf("\n");
      printf("===============================\n");
   
      pthread_mutex_unlock(&mutex);
   }
   printf("Number of vehicles passed from each start point\n");	
   printf("P1 : %d times\n", passed_cnt[1]);
   printf("P2 : %d times\n", passed_cnt[2]);
   printf("P3 : %d times\n", passed_cnt[3]);
   printf("P4 : %d times\n", passed_cnt[4]);
   printf("Total time : %d ticks\n", tick);
   
   return 0;
}

void *path_go1(void *null) {
   while(true) {
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&cond1, &mutex); //wait
      //대기 -> 출발 연산
      for(int i=wait_front; i<wait_end; i++) {
         if(waiting_queue[i]==1) {
            passing_queue[pass_end]=waiting_queue[i];
            wait_cnt[0]--;
            wait_cnt[waiting_queue[i]]--;
            pass_end++;
            for(int j=i; j>wait_front; j--)
               waiting_queue[j]=waiting_queue[j-1];
            wait_front++;
            break;
         }
      }   
      pthread_cond_signal(&cond0);
      pthread_cond_wait(&cond1, &mutex);
      printf("Passed  Vehicle\n");
      printf("Car 1\n");
      pthread_cond_signal(&cond0);
      
      pthread_cond_wait(&cond1, &mutex);
      passed=passing_queue[pass_front];
      passed_cnt[0]++;
      passed_cnt[passed]++;
      pass_front++;
      passed=0;
      pthread_cond_signal(&cond0);
      pthread_mutex_unlock(&mutex);
   }
}

void *path_go2(void *null) {
   while(true) {
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&cond2, &mutex); //wait
      
      //대기 -> 출발 연산
      for(int i=wait_front; i<wait_end; i++) {
         if(waiting_queue[i]==2) {
            passing_queue[pass_end]=waiting_queue[i];
            wait_cnt[0]--;
            wait_cnt[waiting_queue[i]]--;
            pass_end++;
            for(int j=i; j>wait_front; j--)
               waiting_queue[j]=waiting_queue[j-1];
            wait_front++;
            break;
         }
      }   
      pthread_cond_signal(&cond0);
      pthread_cond_wait(&cond2, &mutex);
      printf("Passed  Vehicle\n");
      printf("Car 2\n");
      
      pthread_cond_signal(&cond0);
      pthread_cond_wait(&cond2, &mutex);
      passed=passing_queue[pass_front];
      passed_cnt[0]++;
      passed_cnt[passed]++;
      pass_front++;
      passed=0;
      pthread_cond_signal(&cond0);
      pthread_mutex_unlock(&mutex);
   }
}

void *path_go3(void *null) {
   while(true) {
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&cond3, &mutex); //wait
      
      //대기 -> 출발 연산
      for(int i=wait_front; i<wait_end; i++) {
         if(waiting_queue[i]==3) {
            passing_queue[pass_end]=waiting_queue[i];
            wait_cnt[0]--;
            wait_cnt[waiting_queue[i]]--;
            pass_end++;
            for(int j=i; j>wait_front; j--)
               waiting_queue[j]=waiting_queue[j-1];
            wait_front++;
            break;
         }
      }   
      pthread_cond_signal(&cond0);
      pthread_cond_wait(&cond3, &mutex);
      printf("Passed  Vehicle\n");
      printf("Car 3\n");
      pthread_cond_signal(&cond0);
      
      pthread_cond_wait(&cond3, &mutex);
      passed=passing_queue[pass_front];
      passed_cnt[0]++;
      passed_cnt[passed]++;
      pass_front++;
      passed=0;
      pthread_cond_signal(&cond0);
      pthread_mutex_unlock(&mutex);
   }
}

void *path_go4(void *null) {
   while(true) {
      pthread_mutex_lock(&mutex);
      pthread_cond_wait(&cond4, &mutex); //wait
      
      //대기 -> 출발 연산
      for(int i=wait_front; i<wait_end; i++) {
         if(waiting_queue[i]==4) {
            passing_queue[pass_end]=waiting_queue[i];
            wait_cnt[0]--;
            wait_cnt[waiting_queue[i]]--;
            pass_end++;
            for(int j=i; j>wait_front; j--)
               waiting_queue[j]=waiting_queue[j-1];
            wait_front++;
            break;
         }
      }   
      pthread_cond_signal(&cond0);
      pthread_cond_wait(&cond4, &mutex);
      printf("Passed  Vehicle\n");
      printf("Car 4\n");
      pthread_cond_signal(&cond0);
      
      pthread_cond_wait(&cond4, &mutex);
      passed=passing_queue[pass_front];
      passed_cnt[0]++;
      passed_cnt[passed]++;
      pass_front++;
      passed=0;
      pthread_cond_signal(&cond0);
      pthread_mutex_unlock(&mutex);
   }
}
