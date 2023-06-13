//Including necessary libraries
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <semaphore.h>

#define false 0
#define true 1

sem_t C_sem, N_sem, S_sem, Th_sem, O_sem, CO2_sem, NO2_sem, SO2_sem, THO2_sem,semAtomAmount,semInfo;

int Total_Number_of_C = 0; // Total number of carbon atoms 
int Total_Number_of_N = 0; // Total number of nitrogen atoms 
int Total_Number_of_S = 0; // Total number of sulfur atoms 
int Total_Number_of_TH = 0; // Total number of thorium atoms 
int Total_Number_of_O = 0; // Total number of oxygen atoms 

int generation_time = 100; // Rate of generation time 
int flag_of_CO2 = false;
int MAX_ATOMS = 60;
int atom_created_count = 0; 
char molecule_type[5] = "";

int Number_of_C ; // Total number of carbon atoms will be generated
int Number_of_N ; // Total number of nitrogen atoms will be generated
int Number_of_S ; // Total number of sulfur atoms will be generated
int Number_of_TH; // Total number of thorium atoms will be generated
int Number_of_O ; // Total number of oxygen atoms will be generated

pthread_t composer_CO2_thread;
pthread_t composer_SO2_thread;
pthread_t composer_NO2_thread;
pthread_t composer_THO2_thread;

pthread_t generator_C_thread;
pthread_t generator_S_thread;
pthread_t generator_N_thread;
pthread_t generator_TH_thread;
pthread_t generator_O_thread;

pthread_t print_info_thread;


double generate_exponential() {
    double x = (double) rand() / RAND_MAX; // Generate random number between 0 and 1
    return -1 * (1 / generation_time) * log(1 - x); // Compute exponential distribution
}

struct information info;//Define information variable

// Struct to represent atoms
struct atom {
    int atomID;
    char atomTYPE; // C, N, S ,O or Th
};

// Struct to represent information variable
struct information {
    int atomID;
    char molecule_type[5];
};


void *print_info(void *arg){
    while (true)
    {
        sem_wait(&semInfo);
        printf("Composed molecule: %s\n", info.molecule_type);
        strcpy(info.molecule_type,"");
        sem_post(&semInfo);
    }
    
}

void *Produce_C(void *arg) {
    sem_wait(&semAtomAmount);//waiting atom proccess semaphore
    double waitTime =generate_exponential();//calculate waiting time
    Total_Number_of_C++;
    printf("C with ID: 0 is created.\n"); //I did not write with using atom structure because It does not effect anything.
    sem_post(&semAtomAmount);//unlock atom proccess semaphore
    sleep(waitTime);//sleep 
    
}
//other producer threads are same with C
void *Produce_N(void *arg) {
    sem_wait(&semAtomAmount);
    double waitTime =generate_exponential();
    Total_Number_of_N++;
    printf("N with ID: 1 is created.\n"); //I did not write with using atom structure because It does not effect anything.
    sem_post(&semAtomAmount);
    sleep(waitTime);
    
}

void *Produce_S(void *arg) {
    sem_wait(&semAtomAmount);
    double waitTime =generate_exponential();
    Total_Number_of_S++;
    printf("S with ID: 2 is created.\n"); //I did not write with using atom structure because It does not effect anything.
    sem_post(&semAtomAmount);
    sleep(waitTime);
    
}

void *Produce_Th(void *arg) {
    sem_wait(&semAtomAmount);
    double waitTime =generate_exponential();
    Total_Number_of_TH++;
    printf("TH with ID: 3 is created.\n"); //I did not write with using atom structure because It does not effect anything.
    sleep(waitTime);
    sem_post(&semAtomAmount);
}

void *Produce_O(void *arg) {
    sem_wait(&semAtomAmount);
    double waitTime =generate_exponential();
    Total_Number_of_O   ++;
    printf("O with ID: 4 is created.\n"); //I did not write with using atom structure because It does not effect anything.
    sem_post(&semAtomAmount);
    sleep(waitTime); 
}

void *composer_CO2(void *arg) {
    while (1) {
        sem_wait(&C_sem);//waiting C and O semaphores
        sem_wait(&O_sem);
        while (Total_Number_of_C < 1 || Total_Number_of_O < 2) {}//Waiting needed atoms
        sem_wait(&semAtomAmount);//waiting atom proccess semaphore
        Total_Number_of_C -= 1;
        Total_Number_of_O -= 2;
        printf("Composed molecule: CO2\n");
        sem_post(&semAtomAmount);
        sem_post(&O_sem);//unlock O semaphore

        // Signal other threads according to order
        if(flag_of_CO2 == false){
            sem_post(&N_sem);
            sem_post(&semInfo);
            flag_of_CO2 = true;
        }
        else
        {
            sem_post(&S_sem);
            sem_post(&semInfo);
            flag_of_CO2 = false;
        }
    }
    return NULL;
}
//other composer threads are working like CO2
void *composer_NO2(void *arg) {
    while (1) {
        sem_wait(&N_sem);
        sem_wait(&O_sem);
        while (Total_Number_of_N < 1 || Total_Number_of_O < 2) {}
        sem_wait(&semAtomAmount);
        Total_Number_of_N -= 1;
        Total_Number_of_O -= 2;
        printf("Composed molecule: NO2\n");
        sem_post(&semAtomAmount);
        sem_post(&C_sem);
        sem_post(&O_sem);
    }
    return NULL;
}

void *composer_SO2(void *arg) {
    while (1) {
        sem_wait(&S_sem);
        sem_wait(&O_sem);
        while (Total_Number_of_S < 1 || Total_Number_of_O < 2) {}
        sem_wait(&semAtomAmount);
        Total_Number_of_S -= 1;
        Total_Number_of_O -= 2;
        printf("Composed molecule: SO2\n");
        sem_post(&semAtomAmount);
        sem_post(&Th_sem);
        sem_post(&O_sem);
    }
    return NULL;
}

void *composer_THO2(void *arg) {
    while (1) {
        sem_wait(&Th_sem);
        sem_wait(&O_sem);
        while (Total_Number_of_TH < 1 || Total_Number_of_O < 2) {}
        sem_wait(&semAtomAmount);
        Total_Number_of_TH -= 1;
        Total_Number_of_O -= 2;
        printf("Composed molecule: THO2\n");
        sem_post(&semAtomAmount);
        sem_post(&C_sem);
        sem_post(&O_sem);
    }
    return NULL;
}

int main(int argc, char * argv[])
{
    int opt;
    double waiting_time;

    while ((opt = getopt(argc, argv, "m:g:")) != -1) {
        switch (opt) {
            case 'm':
                MAX_ATOMS = atoi(optarg);
                if(MAX_ATOMS %6 != 0)
                {
                    printf("Value that you are entered is not multiple of 6!!! It is assigned to value for closest value that is multiple of 6.");
                    MAX_ATOMS = (MAX_ATOMS/6)*6;
                    printf("New value is : %d\n",MAX_ATOMS);
                }
                break;
            case 'g':
                generation_time = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-c carbon_atoms] [-n nitrogen_atoms] [-s sulfur_atoms] [-t thorium_atoms] [-o oxygen_atoms] [-g generation_time]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

    }
    Number_of_C = MAX_ATOMS/6; // Total number of carbon atoms will be generated
    Number_of_N = MAX_ATOMS/6; // Total number of nitrogen atoms will be generated
    Number_of_S = MAX_ATOMS/6; // Total number of sulfur atoms will be generated
    Number_of_TH = MAX_ATOMS/6; // Total number of thorium atoms will be generated
    Number_of_O = MAX_ATOMS/3; // Total number of oxygen atoms will be generated

    struct atom atoms[5];
    for (int i = 0; i < 5; i++)//Define Atoms C:0 N:1 S:2 T:3 O:4
    {
        atoms[i].atomID = i;
        if (i==0)
        {
            atoms[i].atomTYPE = 'C';
        }
        else if (i==1)
        {
            atoms[i].atomTYPE = 'N';
        }
        else if (i==2)
        {
            atoms[i].atomTYPE = 'S';
        }
        else if (i==3)
        {
            atoms[i].atomTYPE = 'T';
        }
        else if (i==4)
        {
            atoms[i].atomTYPE = 'O';
        }
        
    }
    
    int conditionGenerate;
    int selectionAtom;
    int waste_flag = false;
    struct atom newAtom;
    
    sem_init(&C_sem,0,1);
    sem_init(&S_sem,0,0);
    sem_init(&N_sem,0,0);
    sem_init(&Th_sem,0,0);
    sem_init(&O_sem,0,1);

    sem_init(&CO2_sem,0,0);
    sem_init(&SO2_sem,0,0);
    sem_init(&THO2_sem,0,0); 
    sem_init(&NO2_sem,0,0);

    sem_init(&semInfo,0,0);
    sem_init(&semAtomAmount,0,1);
  
    

    pthread_create(&composer_CO2_thread,NULL,&composer_CO2,NULL);
    pthread_create(&composer_NO2_thread,NULL,&composer_NO2,NULL);
    pthread_create(&composer_SO2_thread,NULL,&composer_SO2,NULL);
    pthread_create(&composer_THO2_thread,NULL,&composer_THO2,NULL);

    /*pthread_create(&generator_C_thread,NULL,&Produce_C,NULL);
    pthread_create(&generator_N_thread,NULL,&Produce_N,NULL);
    pthread_create(&generator_S_thread,NULL,&Produce_S,NULL);
    pthread_create(&generator_TH_thread,NULL,&Produce_Th,NULL);
    pthread_create(&generator_O_thread,NULL,&Produce_O,NULL);*/
    
    for (int i = 0; i < MAX_ATOMS; i++)
    {
        waiting_time = generate_exponential();
        conditionGenerate = true;
        while(conditionGenerate){
            selectionAtom=(rand() % 5);         // create random number between 0 and 3
            switch(selectionAtom){
                case 0:                         //If it is C,
                    if(Number_of_C == 0){break;}      //If C is no longer need in total, do not set the flag
                    else{
                        newAtom = atoms[0];
                        pthread_create(&generator_C_thread,NULL,&Produce_C,NULL); //Create thread for only once for every time.
                        Number_of_C--;
                    }
                    conditionGenerate=false;     //set the flag so that break that loop then assign it to a tube
                    break;
                //Other cases are made within same idea of Carbon
                case 1:
                    if(Number_of_N == 0){break;}
                    else{
                        newAtom = atoms[1];

                        pthread_create(&generator_N_thread,NULL,&Produce_N,NULL);
                        Number_of_N--;
                        }
                    conditionGenerate=false;
                    break;
                case 2:
                    if(Number_of_S == 0){break;}
                    else{
                        newAtom = atoms[2];
                        pthread_create(&generator_S_thread,NULL,&Produce_S,NULL);
                        Number_of_S--;
                        }
                    conditionGenerate=false;
                    break;
                case 3:
                    if(Number_of_TH == 0){break;}
                    else{
                        newAtom = atoms[3];
                        pthread_create(&generator_TH_thread,NULL,&Produce_Th,NULL);
                        Number_of_TH--;
                        }
                    conditionGenerate=false;
                    break;
                case 4:
                    if(Number_of_O == 0){break;}
                    else{
                        newAtom = atoms[4];
                        
                        pthread_create(&generator_O_thread,NULL,&Produce_O,NULL);
                        Number_of_O--;
                        }
                    conditionGenerate=false;
                    break;
            }
       
        }
        atom_created_count++;
        //printf("C: %d,N: %d,S: %d,TH: %d,O: %d,number_of_all_atoms: %d\n",Total_Number_of_C,Total_Number_of_N,Total_Number_of_S,Total_Number_of_TH,Total_Number_of_O,MAX_ATOMS);
    }
    //printf("C: %d,N: %d,S: %d,TH: %d,O: %d,number_of_all_atoms: %d\n",Total_Number_of_C,Total_Number_of_N,Total_Number_of_S,Total_Number_of_TH,Total_Number_of_O,MAX_ATOMS);
    while (1){
        if(Total_Number_of_O ==0 && Number_of_O ==0)
        {
            return 0;//Thread does not end up until O atoms are finished.
        }
    }

     



}