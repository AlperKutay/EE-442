#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
//define funcitons and global variables
#define THREAD_COUNT 7
#define STACK_SIZE 4096
int currentThreadIndex = 0;// these three variables are going to count threads
int count_finished_threads = 0;
int count_ready_threads = 0;
void initializeThread();
void IO_run();
int SRTF_scheduler ();
void createThread ();
void run_thread (int Thread_index,int Cpu_index,int IO_index);
void print_state(int Thread_index);
void IO_thread(int Thread_index,int count_cpu,int IO_index);
void exitThread(int ThreadIndex);
enum ThreadState {//thread states are defined
    READY,
    RUNNING,
    FINISHED,
    IO,
    EMPTY,
};
struct Info_thread
{
    ucontext_t context;
    int STATE;
    int cpu_brust[3];//I decided that burst times should not be defined one by one because arrays can be used more easily
    int I_O_burst[3];
    int index_IO ;//since I used array to define burst times, I should define index varibales 
    int index_CPU ;
};
struct Info_thread *threadArray[THREAD_COUNT]; 
ucontext_t main_thread;// defined main thread seperatly
void initializeThread(int index,int cpu1,int cpu2,int cpu3, int IO1, int IO2, int IO3){//Init threads 
    threadArray[index]->STATE = EMPTY;
    threadArray[index]->cpu_brust[0] = cpu1;
    threadArray[index]->cpu_brust[1] = cpu2;
    threadArray[index]->cpu_brust[2] = cpu3;
    threadArray[index]->I_O_burst[0] = IO1;
    threadArray[index]->I_O_burst[1] = IO2;
    threadArray[index]->I_O_burst[2] = IO3;
    threadArray[index]->index_IO = 0;
    threadArray[index]->index_CPU = 0;
}
void createThread(void (*function)())//thread creations
{
    for (int threadIndex = 0; threadIndex < THREAD_COUNT; threadIndex++) {//all threads will be created in one funciton

        if (threadArray[threadIndex]->STATE == EMPTY)
        {
            getcontext(&threadArray[threadIndex]->context);
            threadArray[threadIndex]->context.uc_link = &main_thread;// In the creation, all threads linked to main thread
            threadArray[threadIndex]->context.uc_stack.ss_sp = malloc(STACK_SIZE);
            threadArray[threadIndex]->context.uc_stack.ss_size = STACK_SIZE;
            threadArray[threadIndex]->STATE = READY;
        }
        else
        {
            printf("Something could be wrong on Thread : %d",threadIndex+1);
        }
    }
}
void IO_thread(int Thread_index,int count_cpu,int IO_index)
{
    if (threadArray[Thread_index]->STATE == IO)
    {
        threadArray[Thread_index]->I_O_burst[IO_index] = threadArray[Thread_index]->I_O_burst[IO_index]-count_cpu;// I subtract as many as cpu time that will waste
        if(threadArray[Thread_index]->I_O_burst[IO_index] < 1)// If substraction ends up 0 or negative numbers
        {
            threadArray[Thread_index]->I_O_burst[IO_index] = 0;//we make equal to 0 
            if (IO_index == 2 )//If io index is 2 thread is finished
            {
                threadArray[Thread_index]->STATE = FINISHED;
                count_finished_threads++;//increase the count
            }
            else if (IO_index <3 )//If io index is smaller than 2, it means it does not finish so that, increase index_io and change states as READY
            {
                threadArray[Thread_index]->index_IO++;
                threadArray[Thread_index]->STATE = READY;
                count_ready_threads++;//increase ready threads
            }   
        }
    }
    else
    {
        printf("Something wrong on running\n");
    }
}
void running_thread(int Thread_index,int Cpu_index,int IO_index)
{   
    int left_second;
    if (threadArray[Thread_index]->STATE == RUNNING)//Control that thread is in running state
    {
        left_second = threadArray[Thread_index]->cpu_brust[Cpu_index];
        printf("\n");
    }
    else
    {
        printf("Something wrong on running\n");
        return;
    }
    if (left_second>3)//If left second is bigger than 3, cpu burst does not finished in this procces
    {
        for (int i = 0; i < Thread_index; i++)
        {
            printf("\t");
            
        }
        printf("%d\n",left_second-1);
        sleep(1);
        for (int i = 0; i < Thread_index; i++)
        {
            printf("\t");
        }
        printf("%d\n",left_second-2);
        sleep(1);
        for (int i = 0; i < Thread_index; i++)
        {
            printf("\t");
        }
        printf("%d\n",left_second-3);
        threadArray[Thread_index]->STATE = READY;//make ready status again
        count_ready_threads++;//increase ready threads
        threadArray[Thread_index]->cpu_brust[Cpu_index] = threadArray[Thread_index]->cpu_brust[Cpu_index] - 3 ;

    }
    else if(left_second<4)// If cpu burst is going to finish in this procces we should adjustments in cpu index
    {
        for(int i = left_second; i>0 ; i--)
        {
            left_second--;
            threadArray[Thread_index]->cpu_brust[Cpu_index]--;
            for (int i = 0; i < Thread_index; i++)
            {
                printf("\t");
            }
            sleep(1);
            printf("%d\n",left_second);
        }
        if(Cpu_index<2)// since we cannot decide whether thread is finished or not by looking at cpu index, we only increase it
        {
            threadArray[Thread_index]->index_CPU++;

        }
        threadArray[Thread_index]->STATE = IO;//since cpu burst is finished In this procces, we changed it to IO
    }
    
}
void print_space(int num_of_space)// we are using it in print state function
{//This funciton is used for blank adjustments
    for (int i = 0; i < num_of_space; i++)
    {
        printf("   ");
    }
}
void print_state(int Thread_index)//this funciton is used for printing states
{
    int count_running=7,count_finished=7,count_IO=7;
    if(Thread_index != -1)//if thread index is -1 it means thre is not any running threads
    {
        printf("\nrunning>T%d\tready>",Thread_index+1);
    }
    else
    {
        printf("\nrunning>   \tready>");
    }
    for (int i = 0; i < THREAD_COUNT; i++)//print ready threads
    {
        if (threadArray[i]->STATE == READY)
        {
            printf("T%d",i+1);
            printf(",");
            count_running--;
        }
    }
    print_space(count_running);
    printf("  finished>");
    for (int i = 0; i < THREAD_COUNT; i++)//print finished threads
    {
        if (threadArray[i]->STATE == FINISHED)
        {
            printf("T%d",i+1);
            printf(",");
            count_finished--;
        }       
    }
    print_space(count_finished);
    printf("  IO>");
    for (int i = 0; i < THREAD_COUNT; i++)//print IO threads
    {
        if (threadArray[i]->STATE == IO)
        {
            printf("T%d",i+1);
            printf(",");     
            count_IO--;
        }      
    }
    print_space(count_IO);
    printf("\n");
}
void IO_run(int threadIndex)
{
    int First_IO_index = 8;
    int count_cpu;
    if (threadIndex < 8)//If input is not 8, it means there are ready threads to be proccesed
    {//in this conditions we are going to calculate IO time
        int index_cpu = threadArray[threadIndex]->index_CPU;
        int cpu_time= threadArray[threadIndex]->cpu_brust[index_cpu];
        if(cpu_time>2){// If cpu burst time that will be proccesed is bigger than 3, then count time is equal to 3 beacuse cpu time is not finished in this procces
            count_cpu = 3;
        }
        else if (cpu_time < 3)// If cpu burst time that will be proccesed is smaller than 3, then count time is equal to it
        {
            count_cpu = cpu_time;
        }
    }
    else if (threadIndex == 8)//IF input is 8, it means there are no ready threads to be proccesed
    {
        count_cpu = 3;//so that count cpu should be 3
    }
    for (int i = 0; i < THREAD_COUNT; i++)//In my algorithm works with First_IO_index 
    {
        if(threadArray[i]->STATE == IO)//find First_IO_index and runs io
        {
            First_IO_index = i;
            break;
        }
    }
    if (First_IO_index != 8)//if it could find First_IO_index so that runs IO
    {
        int idx_IO = threadArray[First_IO_index]->index_IO;
        makecontext(&threadArray[First_IO_index]->context,(void (*)(void))IO_thread,3,First_IO_index,count_cpu,idx_IO);
        getcontext(&main_thread);
        swapcontext(&main_thread,&threadArray[First_IO_index]->context); 
        if(threadArray[First_IO_index]->STATE == FINISHED)// IF after io procces thread is finished, exit thread
        {
            exitThread(First_IO_index);
        }
    }
}
void exitThread(int ThreadIndex)
{
    if(ThreadIndex != -1)//-1 means main thread
    {
        free(threadArray[ThreadIndex]->context.uc_stack.ss_sp);
    }
    else{
        free(main_thread.uc_stack.ss_sp);
    }
}
int SRTF_scheduler (){
    int Thread_index = -1;
    int Shortest_Time = 10000;
    int idx_cpu;
    for (int i = 0; i < THREAD_COUNT; i++)//check the shortest remaining time thread
    {
        if (threadArray[i]->STATE == READY){
            idx_cpu = threadArray[i]->index_CPU;
            //printf("Index : %d    %d < %d\n",i,threadArray[i]->cpu_brust[idx_cpu],Shortest_Time);
            if (threadArray[i]->cpu_brust[idx_cpu] < Shortest_Time)
            {
                Shortest_Time = threadArray[i]->cpu_brust[idx_cpu];//If find shortest one, than equal it
                Thread_index = i;
            }
        }
    }
    return Thread_index;
}
void runThread(int signum)
{
    int idx_cpu;
    int idx_IO;
    int Thread_index;
    if (count_ready_threads == 0)//If there is not any threads which is in ready state, then print_state(-1) and IO_run(8)
    {
        alarm(3);
        print_state(-1);//if print_state is -1 it means thre is not any running threads
        IO_run(8);//IO time will be 3
        return;
    }
    Thread_index = SRTF_scheduler();//determine the Thread_index
    idx_cpu = threadArray[Thread_index]->index_CPU;//determine the cpu index and io index
    idx_IO = threadArray[Thread_index]->index_IO;
    if (threadArray[Thread_index]->STATE == READY)//control the ready state
    {
        count_ready_threads--;//decrease ready threads because this thread is going to be running state
        threadArray[Thread_index]->STATE = RUNNING;
        print_state(Thread_index);//print state
        IO_run(Thread_index);//run io before running thread because if we would have run it after running thread, new io threads can be used in this IO_run. We do not want this situation
        makecontext(&threadArray[Thread_index]->context,(void (*)(void))running_thread,3,Thread_index,idx_cpu,idx_IO);
        getcontext(&main_thread);
        swapcontext(&main_thread,&threadArray[Thread_index]->context); 
    }
    alarm(3);//after each run set up the alarm again
}
int main(int argc, char ** argv)
{
    signal(SIGALRM,runThread);//set up the alarm
    for(int i=0;i<THREAD_COUNT;i++)//allocate memory for each thread and init each thread
    {
        threadArray[i]=(struct Info_thread *) malloc(sizeof(struct Info_thread));
        initializeThread(i,atoi(argv[i+1]),atoi(argv[i+8]),atoi(argv[i+15]),atoi(argv[i+22]),atoi(argv[i+29]),atoi(argv[i+35]));
        printf("Thread %d CPU1 : %d, CPU2 : %d, CPU3 : %d, IO1 : %d, IO2 : %d, IO3 : %d\n",i+1,atoi(argv[i+1]),atoi(argv[i+8]),atoi(argv[i+15]),atoi(argv[i+22]),atoi(argv[i+29]),atoi(argv[i+35]));
        count_ready_threads++;//increase ready threads after each init
    }
    printf("Threads :\nT1\tT2\tT3\tT4\tT5\tT6\tT7\n");
    createThread(runThread);//create all threads with runThread funciton
    raise(SIGALRM);//raise the alarm and call the funcion for first time
    while (1)
    {
        if(count_finished_threads == 7)//control whether all threads are finished or not
        {//if all of them are finished close the program
            print_state(-1);
            exitThread(-1);
            printf("\n");
            break;
        }
    }
}
/*Thread 1 CPU1 : 5, CPU2 : 7, CPU3 : 7, IO1 : 4, IO2 : 4, IO3 : 5
Thread 2 CPU1 : 6, CPU2 : 5, CPU3 : 6, IO1 : 4, IO2 : 7, IO3 : 7
Thread 3 CPU1 : 4, CPU2 : 6, CPU3 : 4, IO1 : 7, IO2 : 6, IO3 : 7
Thread 4 CPU1 : 4, CPU2 : 7, CPU3 : 6, IO1 : 5, IO2 : 7, IO3 : 5
Thread 5 CPU1 : 7, CPU2 : 4, CPU3 : 5, IO1 : 6, IO2 : 6, IO3 : 4
Thread 6 CPU1 : 4, CPU2 : 5, CPU3 : 5, IO1 : 6, IO2 : 7, IO3 : 5
Thread 7 CPU1 : 6, CPU2 : 5, CPU3 : 6, IO1 : 5, IO2 : 5, IO3 : 6*/