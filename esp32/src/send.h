#include <HTTPClient.h>
int enviado=0;
int enviadados_running=0;
int geradados_running=0;
String apiServer = "https://concentradorsolar.000webhostapp.com/send";

//dados teste
float t1 = 33.2234;
float t2 = 34.2145;
float t3 = 42.1123;
float t4 = 43.1124;
float t5 = 41.6654;
float t6 = 41.6654;
float t7 = 41.6654;
float t8 = 40.5651;
float t9 = 36.1654;
float t10 = 39.6654;
float vento = 8.3104;
float luminosidade = 1274.1142;
float radiacao = 1123.2324;
float vazao = 0.12345678;

int stack_count = 0;

void geradados(void * parameters){
    while(1){
        srand(horario_epoch);
        t1 = (rand() % (55 - 10 + 1)) + 10;
        t1 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t2 = (rand() % (55 - 10 + 1)) + 10;
        t2 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t3 = (rand() % (55 - 10 + 1)) + 10;
        t3 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t4 = (rand() % (55 - 10 + 1)) + 10;
        t4 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t5 = (rand() % (55 - 10 + 1)) + 10;
        t5 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        srand(horario_epoch+1);
        t6 = (rand() % (55 - 10 + 1)) + 10;
        t6 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t7 = (rand() % (55 - 10 + 1)) + 10;
        t7 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t8 = (rand() % (55 - 10 + 1)) + 10;
        t8 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t9 = (rand() % (55 - 10 + 1)) + 10;
        t9 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        t10 = (rand() % (55 - 10 + 1)) + 10;
        t10 += (float)((rand() % (1000 - 300 + 1)) + 300)/147;
        srand(horario_epoch+2);
        vento = (rand() % (19 - 1 + 1)) + 1;
        vento += (float)((rand() % (478 - 200 + 1)) + 150)/147;
        luminosidade = (rand() % (1000 - 0 + 1)) + 0;
        vento = (rand() % (19 - 1 + 1)) + 1;
        vento += (float)((rand() % (478 - 200 + 1)) + 150)/147;
        radiacao = (rand() % (1700 - 100 + 1)) + 1;
        radiacao += (float)((rand() % (478 - 200 + 1)) + 150)/147;
        vazao = (rand() % (400 - 0 + 1)) + 0;
        vazao=vazao/1000;
        vazao += (float)((rand() % (478 - 200 + 1)))/1470;
        vTaskDelay(1000*60);
    }
    vTaskDelete(NULL);
}

void enviadados(void * parameters){
    logtempo();
    Serial.printf("[esp2api] Tarefa iniciada no nucleo %i. \n", xPortGetCoreID());
    enviado=0;
    HTTPClient http;
    while(1){
        if(connection_status=="ok"){
            if(horario_calendario.tm_sec == 0 && !enviado){
                String path = apiServer;
                path+=  "?key="+key;
                path+=  "&tempo="+String(horario_epoch);
                path+=  "&t1="+String(t1, 4);
                path+=  "&t2="+String(t2,4);
                path+=  "&t3="+String(t3,4);
                path+=  "&t4="+String(t4,4);
                path+=  "&t5="+String(t5,4);
                path+=  "&t6="+String(t6,4);
                path+=  "&t7="+String(t7,4);
                path+=  "&t8="+String(t8,4);
                path+=  "&t9="+String(t9,4);
                path+=  "&t10="+String(t10,4);
                path+=  "&vento="+String(vento,4);
                path+=  "&luminosidade="+String(luminosidade,4);
                path+=  "&radiacao="+String(radiacao,4);
                path+=  "&vazao="+String(vazao,4);
                //manda
                Serial.println(path);
                http.begin(path.c_str());

                logtempo();
                Serial.printf("[esp2api] Enviando dados para o servidor da API.\n");

                // Send HTTP GET request
                int httpResponseCode = http.GET();
                
                if (httpResponseCode==200) {
                    logtempo();
                    Serial.print("[esp2api] OK. HTTP Response code: ");
                    Serial.println(httpResponseCode);
                    //String payload = http.getString();
                    //Serial.println(payload);
                } 
                else {
                    logtempo();
                    Serial.print("[esp2api] ERRO. HTTP Response code: ");
                    Serial.println(httpResponseCode);
                    logtempo();
                    Serial.printf("[esp2api] Armazenando dados do timestamp %ld no stack.\n", horario_epoch);
                    //armazena
                    stack_count+=1;
                    String path_stack = "/tmp/stack/";
                    path_stack += String(horario_epoch);
                    writeFile(LITTLEFS, path_stack.c_str(), path.c_str());
                    path_stack="";
                }

                // Free resources
                http.end();
                path="";
                vTaskDelay(100);
                enviado=1;
            }
            else {
                vTaskDelay(100);
                enviado=0;
            }
            if(stack_count > 0){
                //enviadados 
            }
        }
    }
    vTaskDelete(NULL);
}

void start_enviadados(){
    if (!enviadados_running && geradados_running){
        logtempo();
        Serial.println("[SISTEMA] Iniciando a tarefa esp2api.");
        enviadados_running=1;
        xTaskCreatePinnedToCore(
            enviadados,     //task function
            "esp2api",      //task name
            8192,           //stack size
            NULL,           //parameters to pass to the task
            3,              //priority
            NULL,           //task handle
            APP_CPU_NUM);             //cpu id
        return;
    }
}

void start_geradados(){
    if (!geradados_running){
        logtempo();
        Serial.println("[SENSORES] Iniciando a tarefa geraDados.");
        geradados_running=1;
        xTaskCreatePinnedToCore(
            geradados,      //task function
            "geraDados",    //task name
            8192,           //stack size
            NULL,           //parameters to pass to the task
            2,              //priority
            NULL,           //task handle
            APP_CPU_NUM);             //cpu id
        return;
    }
}