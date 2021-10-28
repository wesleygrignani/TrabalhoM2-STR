#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <conio.h>
#include <unistd.h>


#ifndef _POSIX_THREAD_PRIO_INHERIT
//#error "This system does not support priority inherit protection in mutex"
#endif

#include "sensor.h"

//clock_t begin_global, end_global;

pthread_mutex_t lock;
pthread_mutexattr_t  mutexattr_prioinherit;

void init_mutex_inh();

///Função para exibir os dados no diplay quando solicitado
void *atualiza_display()
{
    int reading=0;
    char tecla;
    fflush(stdin);
    while(1)
    {
        if(kbhit())
        {
            tecla = getch();
            switch(tecla)
            {
            case '1':
                reading = 1;
                break;
            case '2':
                reading = 2;
                break;
            default:
                reading = 0;
            }
        }
        if(reading == 1)  //verifica se ocorreu uma pesagem para atualizar o display
        {
            printf("\nINFORMACOES DOS APARTAMENTOS POR ANDAR: ");
            dice_per_floor();
            reading=0;
        }
        if(reading == 2)  //verifica se ocorreu uma pesagem para atualizar o display
        {
            printf("\nINFORMACOES DOS APARTAMENTOS: ");
            dice_per_apartament();
            reading=0;
        }
        else{
            reading = 0;
        }

        usleep(1000 * 1000);
    }
}

typedef struct
{
    int Num;
    pthread_t th_cookingGas, th_electricity, th_water;
} AP;

int main()
{
    //pthread_t th_cookingGas, th_electricity, th_water;
    pthread_t th_display;
    AP apartamento[6];
    pthread_attr_t tattr;
    struct sched_param param;


    printf("\n           SISTEMA DE MONITORAMENTO");
    printf("\n\n1 - OBSERVAR OS DADOS POR ANDAR\n");
    printf("\n\n2 - OBSERVAR OS DADOS POR APARTAMENTO\n\n");

    pthread_mutex_init(&lock, NULL);

    init_mutex_inh();
    /* inicializado com atributos padrão */
    pthread_attr_init (&tattr);

    /* obter o parâmetro de programação existente */
    pthread_attr_getschedparam(&tattr, &param);

    /* definir a prioridade; outros parâmetros não mudaram */
    param.sched_priority = 3;

    pthread_attr_setschedparam (&tattr, &param);

    pthread_create(&apartamento[0].th_electricity,&tattr, (void *)thread_electricity_AP_101, NULL);
    pthread_create(&apartamento[1].th_electricity,&tattr, (void *)thread_electricity_AP_102, NULL);
    pthread_create(&apartamento[2].th_electricity,&tattr, (void *)thread_electricity_AP_201, NULL);
    pthread_create(&apartamento[3].th_electricity,&tattr, (void *)thread_electricity_AP_202, NULL);
    pthread_create(&apartamento[4].th_electricity,&tattr, (void *)thread_electricity_AP_301, NULL);
    pthread_create(&apartamento[5].th_electricity,&tattr, (void *)thread_electricity_AP_302, NULL);

    /* definindo o novo parâmetro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    /* definir a prioridade; outros parâmetros não mudaram */
    param.sched_priority = 4;

    /* definindo o novo parâmetro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    pthread_create(&apartamento[0].th_water, &tattr, (void *)thread_water_Ap_101, NULL);
    pthread_create(&apartamento[1].th_water, &tattr, (void *)thread_water_Ap_102, NULL);
    pthread_create(&apartamento[2].th_water, &tattr, (void *)thread_water_Ap_201, NULL);
    pthread_create(&apartamento[3].th_water, &tattr, (void *)thread_water_Ap_202, NULL);
    pthread_create(&apartamento[4].th_water, &tattr, (void *)thread_water_Ap_301, NULL);
    pthread_create(&apartamento[5].th_water, &tattr, (void *)thread_water_Ap_302, NULL);
    /* definir a prioridade; outros parâmetros não mudaram */

    /* definindo o novo parâmetro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    param.sched_priority = 2;

    /* definindo o novo parâmetro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    pthread_create(&apartamento[0].th_cookingGas,&tattr, (void *)thread_cookingGas_AP_101, NULL);
    pthread_create(&apartamento[1].th_cookingGas,&tattr, (void *)thread_cookingGas_AP_102, NULL);
    pthread_create(&apartamento[2].th_cookingGas,&tattr, (void *)thread_cookingGas_AP_201, NULL);
    pthread_create(&apartamento[3].th_cookingGas,&tattr, (void *)thread_cookingGas_AP_202, NULL);
    pthread_create(&apartamento[4].th_cookingGas,&tattr, (void *)thread_cookingGas_AP_301, NULL);
    pthread_create(&apartamento[5].th_cookingGas,&tattr, (void *)thread_cookingGas_AP_302, NULL);

    pthread_attr_setschedparam (&tattr, &param);

    param.sched_priority = 1;

    /* definindo o novo parâmetro de escalonamento */
    pthread_attr_setschedparam (&tattr, &param);

    pthread_create(&th_display,&tattr,(void *)atualiza_display, NULL);

    while(1)
    {
        usleep(10000);
    }
    for(int i = 0; i<6; i++)
    {
        pthread_join(apartamento[i].th_electricity, NULL);
        pthread_join(apartamento[i].th_water, NULL);
        pthread_join(apartamento[i].th_cookingGas, NULL);
        pthread_join(th_display, NULL);
    }

    return 0;

}

/* função para inicializar o mutex com o protocolo de herança de prioridade */
void init_mutex_inh()
{
    int rtn;
    int mutex_protocol;

    /* Qual é o protocolo padrão no host? - retirado de exemplo*/
    if ((rtn = pthread_mutexattr_init(&mutexattr_prioinherit)) != 0)
        fprintf(stderr,"pthread_mutexattr_init: %s",strerror(rtn)); //mensagem em caso de haver erro
    if ((rtn = pthread_mutexattr_getprotocol(&mutexattr_prioinherit, &mutex_protocol)) != 0)
        fprintf(stderr,"pthread_mutexattr_getprotocol: %s",strerror(rtn)); //mensagem em caso de haver erro

#ifdef DEBUG
    if (mutex_protocol == PTHREAD_PRIO_PROTECT)
        printf("Default mutex protocol is PTHREAD_PRIO_PROTECT\n");
    else if (mutex_protocol == PTHREAD_PRIO_INHERIT)
        printf("Default mutex protocol is PTHREAD_PRIO_INHERIT\n");
    else if (mutex_protocol == PTHREAD_PRIO_NONE)
        printf("Default mutex protocol is PTHREAD_PRIO_NONE\n");
    else
        printf("Default mutex protocol is unrecognized: %d\n");
#endif

    /* Define o protocolo do mutex como INHERIT - herança de prioridade*/
    if ((rtn = pthread_mutexattr_setprotocol(&mutexattr_prioinherit, PTHREAD_PRIO_INHERIT)) != 0)
        fprintf(stderr,"pthread_mutexattr_setprotocol: %s",strerror(rtn)); //mensagem em caso de haver erro

    /* Inicialize mutex com o objeto de atributo */
    if ((rtn = pthread_mutex_init(&lock,&mutexattr_prioinherit)) != 0)
        fprintf(stderr,"pthread_mutexattr_init: %s",strerror(rtn)); //mensagem em caso de haver erro

#ifdef DEBUG
    /*verificação para identificar se o protocolo foi realmente atribuído */
    if ((rtn = pthread_mutexattr_getprotocol(&mutexattr_prioinherit, &mutex_protocol)) != 0)
        fprintf(stderr,"pthread_mutexattr_getprotocol: %s",strerror(rtn)); //mensagem em caso de haver erro

    if (mutex_protocol == PTHREAD_PRIO_PROTECT)
        printf("Default mutex protocol is PTHREAD_PRIO_PROTECT\n");
    else if (mutex_protocol == PTHREAD_PRIO_INHERIT)
        printf("Default mutex protocol is PTHREAD_PRIO_INHERIT\n");
    else if (mutex_protocol == PTHREAD_PRIO_NONE)
        printf("Default mutex protocol is PTHREAD_PRIO_NONE\n");
    else
        printf("Default mutex protocol is unrecognized: %d\n");
#endif
}
