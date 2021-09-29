#include <HTTPClient.h>
int enviado=0;
int enviado_stack=0;
int esp2api_running=0;
int esp2apistack_running=0;
int geradados_running=0;
int server_status=0;
String apiServer = "https://concentradorsolar.000webhostapp.com/send";
String serverStatus = "https://concentradorsolar.000webhostapp.com/status";

int stack_count = 1;

void geradados(void * parameters){
    geradados_running = 1;
    Serial.printf("\n[geraDados] Tarefa iniciada no nucleo %i com prioridade %i. \n", xPortGetCoreID(), uxTaskPriorityGet(NULL));
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
        windspeed_ms = (rand() % (19 - 1 + 1)) + 1;
        windspeed_ms += (float)((rand() % (478 - 200 + 1)) + 150)/147;
        luminosidade = (rand() % (1000 - 0 + 1)) + 0;
        windspeed_ms = (rand() % (19 - 1 + 1)) + 1;
        windspeed_ms += (float)((rand() % (478 - 200 + 1)) + 150)/147;
        radiacao = (rand() % (1700 - 100 + 1)) + 1;
        radiacao += (float)((rand() % (478 - 200 + 1)) + 150)/147;
        flowMilliLitres = (rand() % (400 - 0 + 1)) + 0;
        flowMilliLitres=flowMilliLitres/1000;
        flowMilliLitres += (float)((rand() % (478 - 200 + 1)))/1470;
        vTaskDelay(1000*60);
    }
    geradados_running = 0;
    vTaskDelete(NULL);
}

void enviadados_stack(void * parameters){
    //IMPLEMENTAR:
    //funcao que verifica se há conexão com a net usando método GET
    //quando reconecta, o ESP pausa a tarefa esp2api
    //e manda os dados do stack
    //depois retoma a tarefa esp2api
    esp2apistack_running = 1;
    logtempo();
    Serial.printf("\n[esp2api-stack] Tarefa iniciada no nucleo %i com prioridade %i. \n", xPortGetCoreID(), uxTaskPriorityGet(NULL));
    enviado_stack=0;
    HTTPClient http_stack;
    while(1){
        vTaskDelay(500);
        //HTTPClient http;
        File root = LITTLEFS.open("/tmp/stack"); //local onde serao armazenados os dados
        File i = root.openNextFile();
        String i_name = String("");
        String i_content = String("");
        String i_path = String("");
        http_stack.begin(serverStatus.c_str());
        int httpResponseCode_status = http_stack.GET();
        if (httpResponseCode_status == 200) server_status = 1;
        http_stack.end();
        if(stack_count > 0 && connection_status == "ok" && server_status){ //se nao tiver erro na conexao, executa
            while(i){
                logtempo();
                i_name = i.name();
                i_path = String(i_name);
                //File i_file = LITTLEFS.open(i_path.c_str());
                while (i.available()){
                    i_content=i.readString();
                }
                http_stack.begin(i_content.c_str());
                Serial.printf("[esp2api-stack] Enviando stack ts=%s\n", i.name());
                Serial.printf("[DEBUG] Conteudo: %s", i_content.c_str());
                vTaskDelay(100);

                // Send HTTP GET request
                int httpResponseCode_stack = http_stack.GET();
                
                if (httpResponseCode_stack==200) {
                    logtempo();
                    Serial.printf("[esp2api-stack] OK. HTTP Response code: %d. Stack ts=%s enviado.\n", httpResponseCode_stack, i.name());
                    i = root.openNextFile();
                    deleteFile(LITTLEFS, i_path.c_str());
                } 
                else {
                    logtempo();
                    Serial.printf("[esp2api-stack] ERRO ao enviar ts=%s. HTTP Response code: %d.\n", i.name(), httpResponseCode_stack);
                }
                //i_file.close();
            }
            i.close();
            root.close();
        }
        else{ //se houver problema na conexao, executa
            int delay_stack = 60*1000*3;
            while(delay_stack > 0){ //delay de 3 minutos para esperar o servidor
                delay_stack-=500;
                vTaskDelay(500);    
            }
        }
    }
    esp2apistack_running = 0;
    vTaskDelete(NULL);
}

void enviadados(void * parameters){
    esp2api_running=1;
    logtempo();
    Serial.printf("\n[esp2api] Tarefa iniciada no nucleo %i com prioridade %i. \n", xPortGetCoreID(), uxTaskPriorityGet(NULL));
    enviado=0;
    HTTPClient http;
    while(1){
        if(connection_status=="ok" && ntp_status && server_status){
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
                path+=  "&vento="+String(windspeed_ms,4);
                path+=  "&luminosidade="+String(luminosidade,4);
                path+=  "&radiacao="+String(radiacao,4);
                path+=  "&vazao="+String(flowMilliLitres,4);
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
                    // adicionar algoritmo para verificar o espaço disponivel no sistema
                    // se nao houver espaço necessario, verifica se o SD está montado
                    // se o SD estiver montado, armazena no SD

                    logtempo();
                    Serial.print("[esp2api] ERRO. HTTP Response code: ");
                    Serial.println(httpResponseCode);
                    logtempo();
                    Serial.printf("[esp2api] Armazenando dados do timestamp %ld no stack.\n", horario_epoch);
                    //armazena
                    stack_count+=1;
                    String path_stack = "/tmp/stack/";
                    path_stack += String(horario_epoch);
                    //path_stack += String(".txt");
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
        }
    }
    vTaskDelete(NULL);
}

void start_esp2api(){
    if (!esp2api_running){
        xTaskCreatePinnedToCore(
            enviadados,     //task function
            "esp2api",      //task name
            8192,           //stack size
            NULL,           //parameters to pass to the task
            3,              //priority
            NULL,           //task handle
            APP_CPU_NUM);   //cpu id
    }
    if (!esp2apistack_running){
        xTaskCreatePinnedToCore(
            enviadados_stack,     //task function
            "esp2api-stack",      //task name
            8192,           //stack size
            NULL,           //parameters to pass to the task
            4,              //priority
            NULL,           //task handle
            APP_CPU_NUM);   //cpu id
    }
    return;
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
            5,              //priority
            NULL,           //task handle
            APP_CPU_NUM);             //cpu id
        return;
    }
}