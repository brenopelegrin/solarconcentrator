#include <time.h>
//#include "RTClib.h"
//#include <Wire.h>

//RTC_DS3231 rtc_externo;
//char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

time_t horario_epoch;
struct tm horario_calendario;

const PROGMEM char* ntpServer = "pool.ntp.br";  //pool ntp do brasil
const long gmtOffset_sec =  -3 * 60 * 60;       //correcao para UTC-3
const int daylightOffset_sec = 0;               //não há horário de verão no Brasil

int sync_delay = 10*60*1000; //sincronizar a cada 10 minutos (em milisegundos)
int ntp_running = 0;

void atualizatempo(void * parameters){
    while (1){
        if (ntp_running) {
            //armazena o tempo
        }
    }
    vTaskDelete(NULL);
}

int pegatempo(int tipo){
    //tipo 0 = epoch
    //tipo 1 = hora
    //tipo 2 = minuto
    //tipo 3 = segundo
    //tipo 4 = ano
    //tipo 5 = mes
    //tipo 6 = dia do mes
    switch(tipo){
        case 0: {
            break;
        }
        case 1: {
            break;
        }
        case 2: {
            break;
        }
        case 3: {
            break;
        }
        case 4: {
            break;
        }
        case 5: {
            break;
        }
        case 6: {
            break;
        }
    }
    return 0;
}

void ntpsync_timer(void * parameters){
    while(1){
        getLocalTime(&horario_calendario);
        time(&horario_epoch);
        while(sync_delay > 0){ //ta na hora? senao, continua o timer
            sync_delay-=500;
            vTaskDelay(500);    
        }
        if (sync_delay == 0 && connection_status == "ok"){ //ta na hora da brincadeira!
            Serial.print("[NTP] Obtendo tempo do servidor ");
            Serial.println(ntpServer);
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //configura o tempo do esp
            if(!getLocalTime(&horario_calendario)){
                Serial.println("[NTP] Falha ao obter o tempo.");    //deu ruim
            }
            else{   //deu certo
                time(&horario_epoch);
                Serial.println("[NTP] Tempo sincronizado com sucesso!");
                Serial.print("[NTP] Calendario gregoriano: ");
                Serial.println(&horario_calendario, "%A, %d/%B/%Y %H:%M:%S");
                Serial.print("[NTP] EPOCH time: ");
                Serial.println(horario_epoch);

                sync_delay = 10*60*1000;
                
            }
        }
    }
    ntp_running = 0;
    vTaskDelete(NULL);
}

void start_timesync(){

    sync_delay=0;
    Serial.println("[SISTEMA] Iniciando a tarefa NTPSyncTimer.");
    if (!ntp_running){
        ntp_running = 1;
        xTaskCreatePinnedToCore(
            ntpsync_timer,    //task function
            "NTPSyncTimer",   //task name
            2048,             //stack size
            NULL,             //parameters to pass to the task
            3,                //priority
            NULL,             //task handle
            1);               //cpu id
        return;
    }
    else Serial.println("[SISTEMA] Houve uma tentativa de iniciar a tarefa NTPSyncTimer, mas ela ja esta rodando.");
}