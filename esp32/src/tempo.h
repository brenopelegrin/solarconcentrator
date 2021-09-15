#include <time.h>
#include "RTClib.h"
//#include <Wire.h>

RTC_DS3231 rtc;

//RTC_DS3231 rtc_externo;
//char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

time_t horario_epoch;
struct tm horario_calendario;
DateTime tempo_rtc;

const PROGMEM char* ntpServer = "pool.ntp.br";  //pool ntp do brasil
const long gmtOffset_sec =  -3 * 60 * 60;       //correcao para UTC-3
const int daylightOffset_sec = 0;               //não há horário de verão no Brasil

const char *meses[12] = {"Janeiro",
                    "Fevereiro",
                    "Março",
                    "Abril",
                    "Maio",
                    "Junho",
                    "Julho",
                    "Agosto",
                    "Setembro",
                    "Outubro",
                    "Novembro",
                    "Dezembro"  };
const char *semana[7] = {"Domingo",
                    "Segunda-feira",
                    "Terça-feira",
                    "Quarta-feira",
                    "Quinta-feira",
                    "Sexta-feira",
                    "Sábado",       };

int sync_delay = 15*60*1000; //sincronizar a cada 10 minutos (em milisegundos)
int ntp_running = 0;
int ntp_status = 0;
int atualizatempo_running = 0;
int extrtc_status = 0;

void atualizatempo(void * parameters){
    while (1){
        if (ntp_running) {
            if(!getLocalTime(&horario_calendario)){
                Serial.println("[AtualizaTempo] Falha ao obter o struct tempo local.");    //deu ruim
            }
            if(!time(&horario_epoch)){
                Serial.println("[AtualizaTempo] Falha ao obter o epoch time.");    //deu ruim
            }
        }
        tempo_rtc = rtc.now();
        vTaskDelay(1000);
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
    //tipo 6 = dia do 
    //tipo 7 = dia da semana
    if (extrtc_status){
        switch(tipo){
            case 0: {
                return tempo_rtc.unixtime();
                break;
            }
            case 1: {
                return tempo_rtc.hour();
                break;
            }
            case 2: {
                return tempo_rtc.minute();
                break;
            }
            case 3: {
                return tempo_rtc.second();
                break;
            }
            case 4: {
                return tempo_rtc.year();
                break;
            }
            case 5: {
                return tempo_rtc.month();
                break;
            }
            case 6: {
                return tempo_rtc.day();
                break;
            }
            case 7: {
                return horario_calendario.tm_wday;
                break;
            }
        }
    }
    else{
        switch(tipo){
            case 0: {
                return horario_epoch;
                break;
            }
            case 1: {
               return horario_calendario.tm_hour;
               break;
            }
            case 2: {
                return horario_calendario.tm_min;
               break;
            }
            case 3: {
               return horario_calendario.tm_sec;
               break;
            }
            case 4: {
               return horario_calendario.tm_year+1900;
               break;
            }
            case 5: {
               return horario_calendario.tm_mon+1;
               break;
            }
            case 6: {
                return horario_calendario.tm_mday;
                break;
            }
            case 7: {
                return horario_calendario.tm_wday;
                break;
            }
        }
    }
    return 0;
}

const char* convertetempo(int tipo, int valor){
    //tipo 0 = epoch
    //tipo 1 = hora
    //tipo 2 = minuto
    //tipo 3 = segundo
    //tipo 4 = ano
    //tipo 5 = mes
    //tipo 6 = dia do mes
    //tipo 7 = dia da semana
    switch(tipo){
        case 5: { //mes
            switch (valor){
                return meses[valor-1];
            }
            break;
        }
        case 7: {
            return semana[valor];
            break;
        }
    }
    return 0;
}

void logtempo(){

    int hora = pegatempo(1);
    int minuto = pegatempo(2);
    int segundo = pegatempo(3);
    int dia = pegatempo(6);
    int mes = pegatempo(5);
    int ano = pegatempo(4);

    //Serial.printf("[%s, ", convertetempo(7, pegatempo(7)));

    if (pegatempo(1) < 10) Serial.printf("[0%d:", hora);
    else Serial.printf("[%d:", hora);

    if (pegatempo(2) < 10) Serial.printf("0%d:", minuto);
    else Serial.printf("%d:", minuto);

    if (pegatempo(3) < 10) Serial.printf("0%d de ", segundo);
    else Serial.printf("%d de ", segundo);

    if (pegatempo(6) < 10) Serial.printf("0%d/", dia);
    else Serial.printf("%d/", dia);

    if (pegatempo(5) < 10) Serial.printf("0%d/", mes);
    else Serial.printf("%d/", mes);

    Serial.printf("%d] ", ano);
    return;
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
                ntp_status = 0;
                Serial.println("[NTP] Falha ao obter o tempo.");    //deu ruim
            }
            else{   //deu certo
                time(&horario_epoch);
                rtc.adjust(DateTime(horario_calendario.tm_year+1900, horario_calendario.tm_mon+1, horario_calendario.tm_mday, horario_calendario.tm_hour, horario_calendario.tm_min, horario_calendario.tm_sec));
                Serial.println("[extRTC] Definindo o horário do RTC externo.");
                Serial.println("[NTP] Tempo sincronizado com sucesso!");
                Serial.print("[NTP] Calendario gregoriano: ");
                Serial.println(&horario_calendario, "%A, %d/%B/%Y %H:%M:%S");
                Serial.print("[NTP] EPOCH time: ");
                Serial.println(horario_epoch);
                ntp_status = 1;
                sync_delay = 10*60*1000;
                if(!atualizatempo_running){
                    xTaskCreatePinnedToCore(
                    atualizatempo,    //task function
                    "AtualizaTempo",   //task name
                    1024,             //stack size
                    NULL,             //parameters to pass to the task
                    2,                //priority
                    NULL,             //task handle
                    PRO_CPU_NUM);               //cpu id
                }
            }
        }
    }
    ntp_running = 0;
    vTaskDelete(NULL);
}

void start_timesync(){

    sync_delay=0;
    Serial.println("[SISTEMA] Iniciando o RTC externo (DS3231).");

    if (! rtc.begin()) {
        Serial.println("[SISTEMA] Não foi possível encontrar o RTC externo.");
        Serial.flush();
        extrtc_status=0;
    }
    else {
        Serial.println("[SISTEMA] RTC externo iniciado.");
        extrtc_status=1;
    }

    Serial.println("[SISTEMA] Iniciando a tarefa NTPSyncTimer.");
    if (!ntp_running){
        xTaskCreatePinnedToCore(
            ntpsync_timer,    //task function
            "NTPSyncTimer",   //task name
            2048,             //stack size
            NULL,             //parameters to pass to the task
            3,                //priority
            NULL,             //task handle
            PRO_CPU_NUM);               //cpu id
        ntp_running=1;
        return;
    }
    else Serial.println("[SISTEMA] Houve uma tentativa de iniciar a tarefa NTPSyncTimer, mas ela ja esta rodando.");
}