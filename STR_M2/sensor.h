/*
Arquivo: sensor.h
Autor: Felipe Viel
Função do arquivo: biblioteca com parametros e protótipos das funções do módulo sensor com implementação de monitor
Criado em 28 de outubro de 2020
Modificado em 28 de outubro de 2020
*/

#ifndef SENSOR_H
#define SENSOR_H

#include <pthread.h>
#include <math.h>

/*
static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t alarme = PTHREAD_COND_INITIALIZER;

static double sensor_lido = 0;
static double limite_atual = HUGE_VAL;
*/


//void sensor_put(double lido);
//double sensor_get(void);
//void sensor_alarme(double limite);

void dice_per_floor();
void dice_per_apartament();

void *thread_electricity_AP_101( );
void *thread_water_Ap_101( );
void *thread_cookingGas_AP_101( );

void *thread_electricity_AP_102( );
void *thread_water_Ap_102( );
void *thread_cookingGas_AP_102( );


void *thread_electricity_AP_201( );
void *thread_water_Ap_201( );
void *thread_cookingGas_AP_201( );


void *thread_electricity_AP_202( );
void *thread_water_Ap_202( );
void *thread_cookingGas_AP_202( );

void *thread_electricity_AP_301( );
void *thread_water_Ap_301( );

void *thread_cookingGas_AP_301( );


void *thread_electricity_AP_302( );
void *thread_water_Ap_302( );
void *thread_cookingGas_AP_302( );



#endif /*SENSOR_H*/
