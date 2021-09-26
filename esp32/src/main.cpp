
#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>
#include <locale.h>

#include "arquivos.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#define BUZZER_PIN 18
#define BUZZER_CHANNEL 0

//credenciais da rede AP
const char* ssid_ap = "Concentrador Solar";
const char* pwd_ap =  "";

//masterkey hardcoded
const String key = "ALDEIDO";

String ssid_host = "null";
String pwd_host =  "null";

//variaveis de controle
String connection_status = "null";
int auth = 0;
int extSD_status = 0;

//task handlers
//TaskHandle_t conectaWifi_h = NULL;

//outras partes do programa
#include "tempo.h"
#include "envia.h"
#include "rastreamento.h"
#include "sensores.h"
 
AsyncWebServer server(80);

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("\n[WiFi-event] event: %d\n", event);

  switch (event) {
    case SYSTEM_EVENT_WIFI_READY: 
      Serial.println("WiFi interface ready");
      break;
    case SYSTEM_EVENT_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case SYSTEM_EVENT_STA_START:
      Serial.println("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      Serial.println("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      Serial.println("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case SYSTEM_EVENT_AP_START:
      Serial.println("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      Serial.println("WiFi access point  stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      Serial.println("Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      Serial.println("Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      Serial.println("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      Serial.println("Received probe request");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      Serial.println("IPv6 is preferred");
      break;
    case SYSTEM_EVENT_ETH_START:
      Serial.println("Ethernet started");
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("Ethernet stopped");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("Ethernet connected");
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("Ethernet disconnected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.println("Obtained IP address");
      break;
    default: break;
}}

void auth_timer(void * parameters){
  int auth_delay = 60000;
  while(auth_delay > 0){
    auth_delay-=500;
    vTaskDelay(500);
  }
  auth = 0;
  logtempo();
  Serial.println("[WEB] Autenticacao expirada apos 1 minuto.");
  vTaskDelete(NULL);
}

void conecta_host(void * parameters){
  
  WiFi.disconnect(true);
  Serial.println("[CREDENCIAIS - STATION]");
  Serial.print("SSID: ");
  Serial.println(ssid_host);
  Serial.print("PASSWORD: ");
  Serial.println(pwd_host);
  Serial.println("\n[WIFI] Conectando ao WiFi Station. [15 seg]");
  WiFi.begin(ssid_host.c_str(), pwd_host.c_str());
  connection_status="connecting";
  int tempo = 15000;
  while (WiFi.status() != WL_CONNECTED && tempo > 0) {
    tempo=tempo-500;
    vTaskDelay(500);
  }
  if (tempo <=0 && WiFi.status() != WL_CONNECTED){
    Serial.println("[WIFI] ERRO: Tempo de conexao esgotado. Retornando ao modo AP.");
    WiFi.disconnect(true);
    connection_status="fail";
    ssid_host = "null";
    pwd_host = "null";
    vTaskDelete(NULL);
  }

  connection_status="ok";
  Serial.println("[WIFI] Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(" ");

  logtempo();
  Serial.println("[LITTLEFS] Salvando SSID da rede em /syscfg/ssid_host.txt");
  writeFile(LITTLEFS, "/syscfg/ssid_host.txt", ssid_host.c_str());
  logtempo();
  Serial.println("[LITTLEFS] Salvando PASSWORD da rede em /syscfg/pwd_host.txt");
  writeFile(LITTLEFS, "/syscfg/pwd_host.txt", pwd_host.c_str());

  start_timesync();
  start_enviadados();

  vTaskDelete(NULL);
}

void handler_webconfigwifi(){
  //colocar as condicoes do server.on em start_server aqui
  return;
}

String dashboard_processor(const String& var)
{
  if(var == "PLACEHOLDER_TEMPERATURA")
    return F("Hello world!");
  return String();
}

void start_server(){
  
  //paginas
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/dashboard");
  });
  server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/dashboard.html", String(), false, dashboard_processor);
  });

  server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request){
    if (auth == 0) request->send(LITTLEFS, "/wifi_no_auth.html");
    if (auth == 1) request->send(LITTLEFS, "/wifi_ok_auth.html");
  });
  
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    time(&horario_epoch);
    struct tm *info;
    info = localtime(&horario_epoch);
    request->send(200, "text/plain", asctime(info));
  });

  //estilos

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/favicon.ico");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/style.css");
  });
  server.on("/dashboard.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/dashboard.css");
  });
  server.on("/files/font-awesome.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/font-awesome.min.css");
  });
  server.on("/files/roboto.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/roboto.css");
  });

  //imagens parte wifi
  server.on("/files/gota.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/gota.png");
  });
  server.on("/files/header_linhas.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/header_linhas.png");
  });
  server.on("/files/icon_consulta.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/icon_consulta.png");
  });
  server.on("/files/success.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/success.png");
  });
  server.on("/files/error.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/error.png");
  });
  server.on("/files/connecting.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/connecting.png");
  });
  server.on("/files/lock.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/lock.png");
  });
  server.on("/files/shield.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/shield.png");
  });

  //imagens dashboard
  server.on("/files/sdcard.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/sdcard.png", "image/png");
  });
  server.on("/files/mundo.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/mundo.png", "image/png");
  });
  server.on("/files/ntp.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/ntp.png", "image/png");
  });
  server.on("/files/cpu.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/cpu.png", "image/png");
  });
  server.on("/files/sinal-wifi.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/sinal-wifi.png", "image/png");
  });
  server.on("/files/system.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/system.png","image/png");
  });
  server.on("/files/ram.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/ram.png","image/png");
  });
  server.on("/files/piranometro.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/piranometro.png", "image/png");
  });
  server.on("/files/local-network.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/local-network.png", "image/png");
  });
  server.on("/files/anenometro.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/anenometro.png", "image/png");
  });
  server.on("/files/database.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/database.png", "image/png");
  });
  server.on("/files/vazao.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/vazao.png", "image/png");
  });
  server.on("/files/termometro.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/termometro.png", "image/png");
  });

  //fontes
  server.on("/files/Metropolis-Light.otf", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/Metropolis-Light.otf");
  });
  server.on("/files/roboto-v27-latin-900.eot", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/roboto-v27-latin-900.eot");
  });
  server.on("/files/roboto-v27-latin-900.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/roboto-v27-latin-900.svg");
  });
  server.on("/files/roboto-v27-latin-900.ttf", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/roboto-v27-latin-900.ttf");
  });

  server.on("/wifi/connecting", HTTP_GET, [](AsyncWebServerRequest *request){
    if(connection_status == "ok"){
      request->send(LITTLEFS, "/wifi_connected.html");
    }
    if(connection_status == "fail"){
      request->send(LITTLEFS, "/wifi_invalid_network.html");
    }
    if(connection_status == "connecting"){
      request->send(LITTLEFS, "/wifi_connecting.html");
    }
    if(connection_status == "null"){
      request->redirect("/wifi");
    }
  });
  
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
    logtempo();
    Serial.print("\n[WEB] Numero de parametros recebidos: ");
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
 
        AsyncWebParameter* p = request->getParam(i);
        if (p->name() == "key" && !auth){

            if (p->value() == key){

                auth=1; //autentica
                //NOTA: criar handler para essa tarefa e matar ela aqui, antes de começar outra pois se começar mais uma o esp crasha
                xTaskCreatePinnedToCore(
                  auth_timer,       //task function
                  "AuthTimer",      //task name
                  1024,             //stack size
                  NULL,             //parameters to pass to the task
                  4,                //priority
                  NULL,             //task handle
                  PRO_CPU_NUM);               //cpu id

                Serial.println("[QUERY RECEBIDA]");
                logtempo();
                Serial.println("[WEB] Autenticado. Iniciando timer de 1 minuto.\n");
            }
        }

        if (p->name() == "ssid_host" && auth){

            Serial.println("[QUERY RECEBIDA]");
            Serial.print("Parametro: ");
            Serial.println(p->name());

            ssid_host = p->value(); //pega o valor do parametro

            Serial.print("Valor: ");
            Serial.println(ssid_host);
            Serial.println(" ");
        }

        if (p->name() == "pwd_host" && auth){

            Serial.println("[QUERY RECEBIDA]");
            Serial.print("Parametro: ");
            Serial.println(p->name());

            pwd_host = p->value(); //pega o valor do parametro

            Serial.print("Valor: ");
            Serial.println(pwd_host);
            Serial.println(" ");
        }
    }

    if(auth && ssid_host != "null" && pwd_host != "null" && connection_status!="ok"){
        connection_status="connecting";
        request->redirect("/wifi/connecting");
        delay(1000);
        xTaskCreatePinnedToCore(
          conecta_host,     //task function
          "ConectaWifi",    //task name
          8192,             //stack size
          NULL,             //parameters to pass to the task
          4,                //priority
          NULL,     //task handle
          PRO_CPU_NUM);               //cpu id
    }
    if (!auth){
      request->send(LITTLEFS, "/wifi_invalid_auth.html");
    }

  });

  logtempo();
  Serial.println("[WEB] Ligando o ESPAsyncWebServer");
  server.begin();

  return;
}

void setup(){

  setlocale (LC_ALL, "Portuguese");
  //inicia o Serial para debug
  Serial.begin(115200);  

  // Inicializa LITTLEFS
  // adicionar algoritmo para detectar se o SDcard está conectado
  // se estiver conectado, verifica se há um fs FAT32. Se nao tiver, formata
  // e aí cria a estrutura de diretorios no SD (/tmp, etc)
  if(!LITTLEFS.begin(true)){
    Serial.println("[LITTLEFS] Erro: não foi possível montar o sistema de arquivos. Reiniciando o sistema.");
    ESP.restart();
  }
  else{
    Serial.println("[LITTLEFS] Sistema de arquivos montado. Lista de arquivos:");
    listDir(LITTLEFS, "/", 3);
    if (!LITTLEFS.exists("/tmp")){
      createDir(LITTLEFS, "/tmp");
      createDir(LITTLEFS, "/tmp/stack");
      Serial.println("[LITTLEFS] Criando diretório /tmp/stack");
    }
  }

  if(!SD.begin(SD_CS)) {
    Serial.println("[extSD] Erro: não foi possível montar o sistema de arquivos.");
    extSD_status=0;
  }
  else{
    Serial.println("[extSD] Sistema de arquivos montado. Lista de arquivos:");
    extSD_status=1;
    listDir(SD, "/", 3);
  }

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(ssid_ap);
 
  Serial.println("\n[WIFI - AP]");
  Serial.print("SSID: ");
  Serial.println(ssid_ap);
  Serial.print("PASSWORD: ");
  Serial.println(pwd_ap);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.println(" ");

  Serial.println("[WIFI - STATION]");
  Serial.print("SSID: ");
  Serial.println(ssid_host);
  Serial.print("PASSWORD: ");
  Serial.println(pwd_host);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(" ");

  if (LITTLEFS.exists("/syscfg")){

    File syscfg_ssid = LITTLEFS.open("/syscfg/ssid_host.txt", "r");
    if(syscfg_ssid && !syscfg_ssid.isDirectory()){
      ssid_host = "";
      while (syscfg_ssid.available()){
        ssid_host += char(syscfg_ssid.read());
      }
      syscfg_ssid.close();
      Serial.print("[LITTLEFS] SSID da rede encontrada em /syscfg/ssid_host.txt. SSID: ");
      printFile(LITTLEFS, "/syscfg/ssid_host.txt");
    }

    File syscfg_pwd = LITTLEFS.open("/syscfg/pwd_host.txt", "r");
    if(syscfg_pwd && !syscfg_pwd.isDirectory()){
      pwd_host = "";
      while (syscfg_pwd.available()){
        pwd_host += char(syscfg_pwd.read());
      }
      syscfg_pwd.close();
      Serial.print("[LITTLEFS] PASSWORD da rede encontrada em /syscfg/pwd_host.txt.");
      printFile(LITTLEFS, "/syscfg/pwd_host.txt");
    }

    if (ssid_host != "null" && pwd_host != "null" && connection_status != "ok"){
      logtempo();
      Serial.println("[WIFI] Utilizando credenciais armazenadas no LITTLEFS.");
      xTaskCreatePinnedToCore(
          conecta_host,     //task function
          "ConectaWifi",    //task name
          8192,             //stack size
          NULL,             //parameters to pass to the task
          1,                //priority
          NULL,             //task handle
          PRO_CPU_NUM);     //cpu id
    }
  }
  else {
    Serial.println("[LITTLEFS] Criando diretorio /syscfg");
    createDir(LITTLEFS, "/syscfg");
  }

  configSensores();
  start_geradados();  //inicia a task de gerar dados aleatorios (asincrono)

  WiFi.onEvent(WiFiEvent); //quando tiver um evento do wifi, printa (asincrono)
  
  start_server(); //inicia o servidor web (asincrono)

  digitalWrite(18, HIGH);
  delay(1000);
  digitalWrite(18, LOW);

}
 
void loop(){

}