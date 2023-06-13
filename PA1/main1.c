//Including necessary libraries
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <getopt.h>
#include <string.h>

#define NUM_ATOMS 4
#define NUM_COMPOSER 3
#define false 0
#define true 1

#define MAX_ATOMS 100 // Maximum number of atoms
#define MAX_MOLECULES 5 // Number of molecules


int Number_of_C = 20; // Total number of carbon atoms will be generated
int Number_of_N = 20; // Total number of nitrogen atoms will be generated
int Number_of_S = 20; // Total number of sulfur atoms will be generated
int Number_of_TH = 20; // Total number of thorium atoms will be generated
int Number_of_O = 20; // Total number of oxygen atoms will be generated

int Total_Number_of_C = 0; // Total number of carbon atoms 
int Total_Number_of_N = 0; // Total number of nitrogen atoms 
int Total_Number_of_S = 0; // Total number of sulfur atoms 
int Total_Number_of_TH = 0; // Total number of thorium atoms 
int Total_Number_of_O = 0; // Total number of oxygen atoms 

int generation_time = 100; // Rate of generation time 
char molecule_type[5] = "";
int atom_created_count = 0; 
int flag_of_CO2 = false;

pthread_mutex_t mutex_c ; // Mutex for carbon atoms
pthread_mutex_t mutex_n ; // Mutex for nitrogen atoms
pthread_mutex_t mutex_s ; // Mutex for sulfur atoms
pthread_mutex_t mutex_th ; // Mutex for thorium atoms
pthread_mutex_t mutex_o ; // Mutex for oxygen atoms
pthread_mutex_t mutex_info ; // Mutex for information variable

pthread_cond_t cond_c ; // Condition variable for carbon atoms
pthread_cond_t cond_n ; // Condition variable for nitrogen atoms
pthread_cond_t cond_s ; // Condition variable for sulfur atoms
pthread_cond_t cond_th ; // Condition variable for thorium atoms
pthread_cond_t cond_o ; // Condition variable for oxygen atoms
pthread_cond_t cond_info ; // Condition variable for oxygen atoms


pthread_t composer_CO2_thread;
pthread_t composer_SO2_thread;
pthread_t composer_NO2_thread;
pthread_t composer_THO2_thread;
pthread_t print_info_thread;

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

struct information info;//Define information variable

double generate_exponential() {
    double x = (double) rand() / RAND_MAX; // Generate random number between 0 and 1
    return -1 * (1 / generation_time) * log(1 - x); // Compute exponential distribution
}

void *composer_CO2(void *arg) {
    while (1) {
        pthread_cond_wait(&cond_c, &mutex_c);
        pthread_mutex_lock(&mutex_o);
        while (Total_Number_of_C < 1 || Total_Number_of_O < 2) {}
        Total_Number_of_C -= 1;
        Total_Number_of_O -= 2;
        pthread_mutex_lock(&mutex_info);
        strcpy(info.molecule_type,"CO2");
        pthread_mutex_unlock(&mutex_info);
        pthread_cond_signal(&cond_info);
        pthread_mutex_unlock(&mutex_c);
        pthread_mutex_unlock(&mutex_o);
        // Signal other threads
        if(flag_of_CO2 == false){
            pthread_cond_signal(&cond_n);
            flag_of_CO2 = true;
        }
        else
        {
            pthread_cond_signal(&cond_s);
            flag_of_CO2 = false;
        }
    }
    return NULL;
}

void *composer_NO2(void *arg) {
    while (1) {
        pthread_cond_wait(&cond_n, &mutex_n);
        pthread_mutex_lock(&mutex_o);
        while (Total_Number_of_N < 1 || Total_Number_of_O < 2) {

        }
        Total_Number_of_N -= 1;
        Total_Number_of_O -= 2;
        pthread_mutex_lock(&mutex_info);
        strcpy(info.molecule_type,"NO2");
        pthread_mutex_unlock(&mutex_info);
        pthread_mutex_unlock(&mutex_n);
        pthread_mutex_unlock(&mutex_o);

        // Signal other threads
        pthread_cond_signal(&cond_c);
        pthread_cond_signal(&cond_info);
    }
    return NULL;
}

void *composer_SO2(void *arg) {
    while (1) {
        pthread_cond_wait(&cond_s, &mutex_s);
        pthread_mutex_lock(&mutex_o);
        while (Total_Number_of_S < 1 || Total_Number_of_O < 2) {
            //pthread_cond_wait(&cond_s, &mutex_s);
            //pthread_cond_wait(&cond_o, &mutex_o);
        }
        Total_Number_of_S -= 1;
        Total_Number_of_O -= 2;
        pthread_mutex_lock(&mutex_info);
        strcpy(info.molecule_type,"SO2");
        pthread_mutex_unlock(&mutex_info);
        pthread_mutex_unlock(&mutex_s);
        pthread_mutex_unlock(&mutex_o);

        // Signal other threads
        pthread_cond_signal(&cond_th);
        pthread_cond_signal(&cond_info);
    }
    return NULL;
}

void *composer_THO2(void *arg) {
    while (1) {
        pthread_cond_wait(&cond_th, &mutex_th);
        pthread_mutex_lock(&mutex_o);
        while (Total_Number_of_TH < 1 || Total_Number_of_O < 2) {
            //pthread_cond_wait(&cond_th, &mutex_th);
            //pthread_cond_wait(&cond_o, &mutex_o);
        }
        Total_Number_of_TH -= 1;
        Total_Number_of_O -= 2;
        pthread_mutex_lock(&mutex_info);
        strcpy(info.molecule_type,"THO2");
        pthread_mutex_unlock(&mutex_info);
        pthread_mutex_unlock(&mutex_th);
        pthread_mutex_unlock(&mutex_o);

        // Signal other threads
        pthread_cond_signal(&cond_c);
        pthread_cond_signal(&cond_info);
    }
    return NULL;
}
void *print_info(void *arg){
    while (true)
    {
        pthread_cond_wait(&cond_info,&mutex_info);
        printf("Composed molecule: %s\n", info.molecule_type);
        strcpy(info.molecule_type,"");
        pthread_mutex_unlock(&mutex_info);
        
    }
    
}


int main(int argc, char * argv[]){

    int opt;
    double waiting_time;

    while ((opt = getopt(argc, argv, "c:n:s:t:o:g:")) != -1) {
        switch (opt) {
            case 'c':
                Number_of_C = atoi(optarg);
                break;
            case 'n':
                Number_of_N = atoi(optarg);
                break;
            case 's':
                Number_of_S = atoi(optarg);
                break;
            case 't':
                Number_of_TH = atoi(optarg);
                break;
            case 'o':
                Number_of_O = atoi(optarg);
                break;
            case 'g':
                generation_time = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-c carbon_atoms] [-n nitrogen_atoms] [-s sulfur_atoms] [-t thorium_atoms] [-o oxygen_atoms] [-g generation_time]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    //init atoms structures
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
    //Mutex inits
    pthread_mutex_init(&mutex_c,NULL);
    pthread_mutex_init(&mutex_s,NULL);
    pthread_mutex_init(&mutex_n,NULL);
    pthread_mutex_init(&mutex_o,NULL);
    pthread_mutex_init(&mutex_th,NULL);
    pthread_mutex_init(&mutex_info,NULL);
    //COnd inits
    pthread_cond_init(&cond_c,NULL);
    pthread_cond_init(&cond_o,NULL);
    pthread_cond_init(&cond_n,NULL);
    pthread_cond_init(&cond_th,NULL);
    pthread_cond_init(&cond_s,NULL);
    pthread_cond_init(&cond_info,NULL);
    //Threads init
    pthread_create(&composer_CO2_thread,NULL,&composer_CO2,NULL);
    pthread_create(&composer_NO2_thread,NULL,&composer_NO2,NULL);
    pthread_create(&composer_SO2_thread,NULL,&composer_SO2,NULL);
    pthread_create(&composer_THO2_thread,NULL,&composer_THO2,NULL);
    pthread_create(&print_info_thread,NULL,&print_info,NULL);
    //send first signal to Composer Co2
    pthread_cond_signal(&cond_c);
    int conditionGenerate;
    int number_of_all_atoms = Number_of_C+Number_of_N+Number_of_O+Number_of_S+Number_of_TH;
    int selectionAtom;
    int waste_flag = false;
    struct atom newAtom;

    
    for (int i = 0; i < number_of_all_atoms; i++)
    {
        waiting_time = generate_exponential();
        conditionGenerate = true;
        while(conditionGenerate){
            selectionAtom=(rand() % 5);         // create random number between 0 and 3
            switch(selectionAtom){
                case 0:                         //If it is C,
                    if(Number_of_C==0){break;}      //If C is no longer need in total, do not set the flag
                    else{
                        newAtom = atoms[0];
                        Number_of_C--;              //decrement the number of C in total
                        if(Total_Number_of_C <2) //I decided that after 2 atoms, the atom generated will be wasted.
                        {
                            Total_Number_of_C ++;
                        }    
                        else 
                            waste_flag = true;
                        }
                    conditionGenerate=false;     //set the flag so that break that loop then assign it to a tube
                    break;
                //Other cases are made within same idea of Carbon

                case 1:
                    if(Number_of_N==0){break;}
                    else{
                        newAtom = atoms[1];
                        Number_of_N--;
                        if(Total_Number_of_N < 2)
                        {
                            Total_Number_of_N ++ ;
                        }
                        else
                            waste_flag = true;
                        }
                    conditionGenerate=false;
                    break;
                case 2:
                    if(Number_of_S==0){break;}
                    else{
                        newAtom = atoms[2];
                        Number_of_S--;
                        if(Total_Number_of_S < 2)
                        {
                            Total_Number_of_S ++;
                        }
                        else
                            waste_flag = true;
                        }
                    conditionGenerate=false;
                    break;
                case 3:
                    if(Number_of_TH==0){break;}
                    else{
                        newAtom = atoms[3];
                        Number_of_TH--;
                        if(Total_Number_of_TH < 2)
                        {
                            Total_Number_of_TH ++;
                        }                          
                        else
                            waste_flag = true;
                        }
                    conditionGenerate=false;
                    break;
                case 4:
                    if(Number_of_O==0){break;}
                    else{
                        newAtom = atoms[4];
                        Number_of_O--;
                        Total_Number_of_O ++ ;
                        }
                    conditionGenerate=false;
                    break;
            }

        
        }
        atom_created_count++;
        sleep(waiting_time);
        printf("%c with ID:%d is created. Count: %d\n",newAtom.atomTYPE,newAtom.atomID,atom_created_count);//atom generated message print
        if(waste_flag == true)
            printf("%c with ID:%d is wasted.\n",newAtom.atomTYPE,newAtom.atomID);//atom generated message print
        waste_flag = false;
        //printf("C: %d,N: %d,S: %d,TH: %d,O: %d,number_of_all_atoms: %d\n",Total_Number_of_C,Total_Number_of_N,Total_Number_of_S,Total_Number_of_TH,Total_Number_of_O,number_of_all_atoms);
    }
    while (1){
        if(Total_Number_of_O ==0 && Number_of_O ==0)
        {
            return 0;//Thread does not end up until O atoms are finished.
        }
    }


}