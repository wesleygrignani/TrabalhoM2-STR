#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define ESTAVEL 1
#define INSTAVEL 0

#define TAM 50
#define AP_Num 6
#define average_consumption_Water 2
#define average_consumption_cookingGas 5
#define average_consumption_electricity 7

int sensor_Water= ESTAVEL;
int sensor_electricity = ESTAVEL;
int sensor_cookingGas = ESTAVEL;

//clock_t begin_global, end_global;

pthread_mutex_t lock,lock_1;
pthread_mutexattr_t  mutexattr_prioinherit;

///variaveis de contrle
float average_electricity[AP_Num] = {0},average_water[AP_Num]= {0},average_cookingGas[AP_Num]= {0},
                                    data_electricity[AP_Num][TAM]= {0},data_water[AP_Num][TAM]= {0},data_cookingGas[AP_Num][TAM]= {0},
                                            consumption_Water[6]= {average_consumption_Water},consumption_electricity[6]= {average_consumption_electricity},consumption_cookingGas[6]= {average_consumption_cookingGas};

///Calcula Média das amostras
float average(float v[AP_Num][TAM],int N,int n_AP)
{
    float sum = 0;
    pthread_mutex_lock(&lock_1);
    for(int i = 0; i < N; i++)
    {
        sum += v[n_AP][i];
    }
    sum = sum/TAM;
    pthread_mutex_unlock( &lock_1);
    return sum;
}

///Retorna os dados por andar
void dice_per_floor()
{
    float water=0,gas=0,electricity=0;
    pthread_mutex_lock(&lock);
    for(int i = 1; i <=AP_Num; i++)
    {

        water += average_water[i-1];
        gas += average_cookingGas[i-1];
        electricity += average_electricity[i-1];
        if(i%2==0)
        {
            printf("\n\nAndar: %d",i/2);
            printf("\nWater: %f LITERS\nGas: %f CUBIC METERS\nElectricity: %f WATT/H\n",water,gas,electricity);
            water= 0;
            gas = 0;
            electricity = 0;
        }
    }
    pthread_mutex_unlock(&lock);
}

///Retorna os dados por apartamento
void dice_per_apartament()
{
    pthread_mutex_lock(&lock);
    for(int i = 0; i <AP_Num; i++)
    {
        printf("\n\n\nAndar: %d",i+1);
        printf("\nMEDIA DE CONSUMO: ");
        printf("\nWater: %f LITERS\nGas: %f CUBIC METERS \nElectricity: %f WATT/H\n",average_water[i],average_cookingGas[i],average_electricity[i]);
        printf("\n\n5 ULTIMOS DADOS COLETADOS ");
        for(int j = 0; j <AP_Num-1; j++)
        {
            printf("\n\nWater: %f LITERS\nGas: %f CUBIC METERS\nElectricity: %f WATT/H\n",data_water[i][j],data_cookingGas[i][j],data_electricity[i][j]);
        }
    }
    pthread_mutex_unlock(&lock);
}

///Sensores do APARTAMENTO 1
void *thread_electricity_AP_101( )
{
    int aleatoridade = 0,read_electricity=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);

        if(average_electricity[0] > consumption_electricity[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_electricity[0] = average_consumption_electricity*2 ;

            pthread_mutex_unlock(&lock);
        }
        else if(average_electricity[0] < consumption_electricity[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_electricity[0] = average_consumption_electricity/2;

            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_unlock(&lock);
        read_electricity++;
        data_electricity[0][read_electricity] += aleatoridade;
        if(read_electricity == TAM)
        {
            average_electricity[0] = average(data_electricity,TAM,0);
            read_electricity=0;
            //printf("\nAVERAGE ELETRICITY AP 101: %f Wh",average_electricity[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(100000);
        //printf("\nAREAD ELETRICITY AP 102...");
    }

}

void *thread_water_Ap_101( )
{
    float aleatoridade = 0;
    int read_water = 0;

    while(1)
    {
        srand(time(NULL));
        aleatoridade = (rand()%14);
        if(average_water[0] > consumption_Water[0]*1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[0] = average_consumption_Water*2;
            pthread_mutex_unlock(&lock);
        }
        else if(average_water[0] < consumption_Water[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[0] = average_consumption_Water/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_water++;
        data_water[0][read_water]+= aleatoridade/100;
        if(read_water == TAM)
        {
            average_water[0] = average(data_water,TAM,0);
            read_water=0;
            //printf("\nAVERAGE WATER AP 101: %.2f liters",average_water[0]);
        }
        pthread_mutex_unlock(&lock);

        usleep(500000);
        //printf("\nAREAD WATER AP 101...");
    }
}

void *thread_cookingGas_AP_101( )
{
    float aleatoridade = 0;
    int read_cookingGas=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%24);
        if(average_cookingGas[0] > consumption_cookingGas[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_cookingGas[0] = average_consumption_cookingGas*2;

            pthread_mutex_unlock(&lock);
        }
        else if(average_cookingGas[0] < consumption_cookingGas[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_cookingGas[0] = average_consumption_cookingGas/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_cookingGas++;
        data_cookingGas[0][read_cookingGas] += aleatoridade/1000;
        if(read_cookingGas == TAM)
        {
            average_cookingGas[0] = average(data_cookingGas,TAM,0);
            read_cookingGas=0;
            // printf("\nAVERAGE COCKING GAS AP 101: %f cubic meters ",average_cookingGas[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
        //printf("\nAREAD GAS AP 102...");
    }

}
//////////////////////////////////////////////////

///Sensores do APARTAMENTO 2
void *thread_cookingGas_AP_102( )
{
    float aleatoridade = 0;
    int read_cookingGas=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%24);
        if(average_cookingGas[1] > consumption_cookingGas[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_cookingGas[1] = average_consumption_cookingGas*2;

            pthread_mutex_unlock(&lock);
        }
        else if(average_cookingGas[1] < consumption_cookingGas[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_cookingGas[1] = average_consumption_cookingGas/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_cookingGas++;
        data_cookingGas[1][read_cookingGas] += aleatoridade/1000;
        if(read_cookingGas == TAM)
        {
            average_cookingGas[1] = average(data_cookingGas,TAM,1);
            read_cookingGas=0;
            // printf("\nAVERAGE COCKING GAS AP 101: %f cubic meters ",average_cookingGas[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
        //printf("\nAREAD GAS AP 102...");
    }

}

void *thread_electricity_AP_102( )
{
    int aleatoridade = 0,read_electricity=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);

        if(average_electricity[1] > consumption_electricity[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_electricity[1] = average_consumption_electricity*2 ;

            pthread_mutex_unlock(&lock);
        }
        else if(average_electricity[1] < consumption_electricity[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_electricity[1] = average_consumption_electricity/2;

            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_unlock(&lock);
        read_electricity++;
        data_electricity[1][read_electricity] += aleatoridade;
        if(read_electricity == TAM)
        {
            average_electricity[1] = average(data_electricity,TAM,1);
            read_electricity=0;
            //printf("\nAVERAGE ELETRICITY AP 101: %f Wh",average_electricity[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(100000);
        //printf("\nAREAD ELETRICITY AP 102...");
    }

}

void *thread_water_Ap_102( )
{
    float aleatoridade = 0;
    int read_water = 0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);
        if(average_water[1] > consumption_Water[0]*1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[1] = average_consumption_Water*2;
            pthread_mutex_unlock(&lock);
        }
        else if(average_water[1] < consumption_Water[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[1] = average_consumption_Water/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_water++;
        data_water[1][read_water]+= aleatoridade/100;
        if(read_water == TAM)
        {
            average_water[1] = average(data_water,TAM,1);
            read_water=0;
            //printf("\nAVERAGE WATER AP 101: %.2f liters",average_water[0]);
        }
        pthread_mutex_unlock(&lock);

        usleep(500000);
        //printf("\nAREAD WATER AP 101...");
    }
}
//////////////////////////////////////////////////

///Sensores do APARTAMENTO 3
void *thread_cookingGas_AP_201( )
{
    float aleatoridade = 0;
    int read_cookingGas=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%24);
        if(average_cookingGas[2] > consumption_cookingGas[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_cookingGas[2] = average_consumption_cookingGas*2;

            pthread_mutex_unlock(&lock);
        }
        else if(average_cookingGas[2] < consumption_cookingGas[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_cookingGas[2] = average_consumption_cookingGas/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_cookingGas++;
        data_cookingGas[2][read_cookingGas] += aleatoridade/1000;
        if(read_cookingGas == TAM)
        {
            average_cookingGas[2] = average(data_cookingGas,TAM,2);
            read_cookingGas=0;
            // printf("\nAVERAGE COCKING GAS AP 101: %f cubic meters ",average_cookingGas[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
        //printf("\nAREAD GAS AP 102...");
    }

}

void *thread_electricity_AP_201( )
{
    int aleatoridade = 0,read_electricity=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);

        if(average_electricity[2] > consumption_electricity[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_electricity[2] = average_consumption_electricity*2 ;

            pthread_mutex_unlock(&lock);
        }
        else if(average_electricity[2] < consumption_electricity[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_electricity[2] = average_consumption_electricity/2;

            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_unlock(&lock);
        read_electricity++;
        data_electricity[2][read_electricity] += aleatoridade;
        if(read_electricity == TAM)
        {
            average_electricity[2] = average(data_electricity,TAM,2);
            read_electricity=0;
            //printf("\nAVERAGE ELETRICITY AP 101: %f Wh",average_electricity[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(100000);
        //printf("\nAREAD ELETRICITY AP 102...");
    }

}

void *thread_water_Ap_201( )
{
    float aleatoridade = 0;
    int read_water = 0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);
        if(average_water[0] > consumption_Water[0]*1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[2] = average_consumption_Water*2;
            pthread_mutex_unlock(&lock);
        }
        else if(average_water[2] < consumption_Water[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[2] = average_consumption_Water/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_water++;
        data_water[2][read_water]+= aleatoridade/100;
        if(read_water == TAM)
        {
            average_water[2] = average(data_water,TAM,2);
            read_water=0;
            //printf("\nAVERAGE WATER AP 101: %.2f liters",average_water[0]);
        }
        pthread_mutex_unlock(&lock);

        usleep(500000);
        //printf("\nAREAD WATER AP 101...");
    }
}
//////////////////////////////////////////////////

///Sensores do APARTAMENTO 4
void *thread_cookingGas_AP_202( )
{
    float aleatoridade = 0;
    int read_cookingGas=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%24);
        if(average_cookingGas[3] > consumption_cookingGas[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_cookingGas[3] = average_consumption_cookingGas*2;

            pthread_mutex_unlock(&lock);
        }
        else if(average_cookingGas[3] < consumption_cookingGas[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_cookingGas[3] = average_consumption_cookingGas/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_cookingGas++;
        data_cookingGas[3][read_cookingGas]+= aleatoridade/1000;
        if(read_cookingGas == TAM)
        {
            average_cookingGas[3] = average(data_cookingGas,TAM,3);
            read_cookingGas=0;
            // printf("\nAVERAGE COCKING GAS AP 101: %f cubic meters ",average_cookingGas[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
        //printf("\nAREAD GAS AP 102...");
    }

}

void *thread_electricity_AP_202( )
{
    int aleatoridade = 0,read_electricity=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);

        if(average_electricity[3] > consumption_electricity[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_electricity[3] = average_consumption_electricity*2 ;

            pthread_mutex_unlock(&lock);
        }
        else if(average_electricity[3] < consumption_electricity[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_electricity[3] = average_consumption_electricity/2;

            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_unlock(&lock);
        read_electricity++;
        data_electricity[3][read_electricity] += aleatoridade;
        if(read_electricity == TAM)
        {
            average_electricity[3] = average(data_electricity,TAM,3);
            read_electricity=0;
            //printf("\nAVERAGE ELETRICITY AP 101: %f Wh",average_electricity[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(100000);
        //printf("\nAREAD ELETRICITY AP 102...");
    }

}

void *thread_water_Ap_202( )
{
    float aleatoridade = 0;
    int read_water = 0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);
        if(average_water[3] > consumption_Water[0]*1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[3] = average_consumption_Water*2;
            pthread_mutex_unlock(&lock);
        }
        else if(average_water[3] < consumption_Water[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[3] = average_consumption_Water/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_water++;
        data_water[3][read_water]+= aleatoridade/100;
        if(read_water == TAM)
        {
            average_water[3] = average(data_water,TAM,3);
            read_water=0;
            //printf("\nAVERAGE WATER AP 101: %.2f liters",average_water[0]);
        }
        pthread_mutex_unlock(&lock);

        usleep(500000);
        //printf("\nAREAD WATER AP 101...");
    }
}
/////////////////////////////////////////////////

///Sensores do APARTAMENTO 5
void *thread_cookingGas_AP_301( )
{
    float aleatoridade = 0;
    int read_cookingGas=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%24);
        if(average_cookingGas[4] > consumption_cookingGas[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_cookingGas[4] = average_consumption_cookingGas*2;

            pthread_mutex_unlock(&lock);
        }
        else if(average_cookingGas[4] < consumption_cookingGas[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_cookingGas[4] = average_consumption_cookingGas/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_cookingGas++;
        data_cookingGas[4][read_cookingGas] += aleatoridade/1000;
        if(read_cookingGas == TAM)
        {
            average_cookingGas[4] = average(data_cookingGas,TAM,4);
            read_cookingGas=0;
            // printf("\nAVERAGE COCKING GAS AP 101: %f cubic meters ",average_cookingGas[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
        //printf("\nAREAD GAS AP 102...");
    }

}

void *thread_electricity_AP_301( )
{
    int aleatoridade = 0,read_electricity=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);

        if(average_electricity[4] > consumption_electricity[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_electricity[4] = average_consumption_electricity*2 ;

            pthread_mutex_unlock(&lock);
        }
        else if(average_electricity[4] < consumption_electricity[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_electricity[4] = average_consumption_electricity/2;

            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_unlock(&lock);
        read_electricity++;
        data_electricity[4][read_electricity] += aleatoridade;
        if(read_electricity == TAM)
        {
            average_electricity[4] = average(data_electricity,TAM,4);
            read_electricity=0;
            //printf("\nAVERAGE ELETRICITY AP 101: %f Wh",average_electricity[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(100000);
        //printf("\nAREAD ELETRICITY AP 102...");
    }

}

void *thread_water_Ap_301( )
{
    float aleatoridade = 0;
    int read_water = 0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);
        if(average_water[4] > consumption_Water[0]*1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[4] = average_consumption_Water*2;
            pthread_mutex_unlock(&lock);
        }
        else if(average_water[4] < consumption_Water[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[4] = average_consumption_Water/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_water++;
        data_water[4][read_water]+= aleatoridade/100;
        if(read_water == TAM)
        {
            average_water[4] = average(data_water,TAM,4);
            read_water=0;
            //printf("\nAVERAGE WATER AP 101: %.2f liters",average_water[0]);
        }
        pthread_mutex_unlock(&lock);

        usleep(500000);
        //printf("\nAREAD WATER AP 101...");
    }
}
//////////////////////////////////////////////

///Sensores do APARTAMENTO 6
void *thread_cookingGas_AP_302( )
{
    float aleatoridade = 0;
    int read_cookingGas=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%24);
        if(average_cookingGas[5] > consumption_cookingGas[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_cookingGas[5] = average_consumption_cookingGas*2;

            pthread_mutex_unlock(&lock);
        }
        else if(average_cookingGas[5] < consumption_cookingGas[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_cookingGas[5]= average_consumption_cookingGas/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_cookingGas++;
        data_cookingGas[5][read_cookingGas] += aleatoridade/1000;
        if(read_cookingGas == TAM)
        {
            average_cookingGas[5] = average(data_cookingGas,TAM,5);
            read_cookingGas=0;
            // printf("\nAVERAGE COCKING GAS AP 101: %f cubic meters ",average_cookingGas[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
        //printf("\nAREAD GAS AP 102...");
    }
}

void *thread_electricity_AP_302( )
{
    int aleatoridade = 0,read_electricity=0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);

        if(average_electricity[5] > consumption_electricity[0]*1.5)
        {
            pthread_mutex_lock(&lock);

            consumption_electricity[5] = average_consumption_electricity*2 ;

            pthread_mutex_unlock(&lock);
        }
        else if(average_electricity[5] < consumption_electricity[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            consumption_electricity[5] = average_consumption_electricity/2;

            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_unlock(&lock);
        read_electricity++;
        data_electricity[5][read_electricity] += aleatoridade;
        if(read_electricity == TAM)
        {
            average_electricity[5] = average(data_electricity,TAM,5);
            read_electricity=0;
            //printf("\nAVERAGE ELETRICITY AP 101: %f Wh",average_electricity[0]);
        }
        pthread_mutex_unlock(&lock);
        usleep(100000);
        //printf("\nAREAD ELETRICITY AP 102...");
    }
}

void *thread_water_Ap_302( )
{
    float aleatoridade = 0;
    int read_water = 0;

    while(1)
    {
        srand( time( NULL ) );
        aleatoridade = (rand()%14);
        if(average_water[5] > consumption_Water[0]*1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[5] = average_consumption_Water*2;
            pthread_mutex_unlock(&lock);
        }
        else if(average_water[5] < consumption_Water[0]/1.5)
        {
            pthread_mutex_lock(&lock);
            /*printf("\nUNSTABLE WATER SENSOR");
            printf("\n\n");*/
            consumption_Water[5] = average_consumption_Water/2;
            pthread_mutex_unlock(&lock);
        }

        pthread_mutex_lock(&lock);
        read_water++;
        data_water[5][read_water]+= aleatoridade/100;
        if(read_water == TAM)
        {
            average_water[5] = average(data_water,TAM,5);
            read_water=0;
            //printf("\nAVERAGE WATER AP 101: %.2f liters",average_water[0]);
        }
        pthread_mutex_unlock(&lock);

        usleep(500000);
        //printf("\nAREAD WATER AP 101...");
    }
}
////////////////////////////////////////////////////////////////////////
