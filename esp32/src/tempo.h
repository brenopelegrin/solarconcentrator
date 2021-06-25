#include <time.h>
//#include <Arduino.h>
//#include <freertos/FreeRTOS.h>
//#include <freertos/task.h>
//#include "RTClib.h"
//#include <Wire.h>

//RTC_DS3231 rtc_externo;
char daysOfTheWeek[7][12] = {"Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado"};

const char* ntpServer = "pool.ntp.br";
const long gmtOffset_sec =  -3 * 60 * 60;       //correcao para UTC-3
const int daylightOffset_sec = 0;               //não há horário de verão no Brasil

int sync_delay = 15*60*1000; //sincronizar a cada 15 minutos (em milisegundos)

void ntpsync_timer(void * parameters){
    while(1){
        while(sync_delay > 0){ //ta na hora? senao, continua o timer
            sync_delay-=500;
            vTaskDelay(500);
        }
        if (sync_delay == 0 && connection_status == "ok"){ //ta na hora da brincadeira!
            Serial.print("[NTP] Obtendo tempo do servidor ");
            Serial.println(ntpServer);
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            struct tm timeinfo;
            if(!getLocalTime(&timeinfo)){
                Serial.println("[NTP] Falha ao obter o tempo.");
            }
            else{
                Serial.println("[NTP] Tempo sincronizado com sucesso!");
                Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
                Serial.print("Day of week: ");
                Serial.println(&timeinfo, "%A");
                Serial.print("Month: ");
                Serial.println(&timeinfo, "%B");
                Serial.print("Day of Month: ");
                Serial.println(&timeinfo, "%d");
                Serial.print("Year: ");
                Serial.println(&timeinfo, "%Y");
                Serial.print("Hour: ");
                Serial.println(&timeinfo, "%H");
                Serial.print("Hour (12 hour format): ");
                Serial.println(&timeinfo, "%I");
                Serial.print("Minute: ");
                Serial.println(&timeinfo, "%M");
                Serial.print("Second: ");
                Serial.println(&timeinfo, "%S");

                Serial.println("Time variables");
                char timeHour[3];
                strftime(timeHour,3, "%H", &timeinfo);
                Serial.println(timeHour);
                char timeWeekDay[10];
                strftime(timeWeekDay,10, "%A", &timeinfo);
                Serial.println(timeWeekDay);
                Serial.println();

                sync_delay = 15*60*1000;
                
            }
        }
    }
}

void sync_horario(){

    sync_delay=0;

    xTaskCreatePinnedToCore(
        ntpsync_timer,       //task function
        "NTPSyncTimer",   //task name
        2048,             //stack size
        NULL,             //parameters to pass to the task
        1,                //priority
        NULL,             //task handle
        1);               //cpu id
}