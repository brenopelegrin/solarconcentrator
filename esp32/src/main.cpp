#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#include <func_lfs.h> 

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>

//credenciais da rede AP
const char* ssid_ap = "Concentrador Solar";
const char* pwd_ap =  "";

const String key = "ALDEIDO";

String ssid_host = "null";
String pwd_host =  "null";

//variaveis de controle
String connection_status = "null";
int auth = 0;

//task handlers
//TaskHandle_t conectaWifi_h = NULL;

//outras partes do programa
#include "tempo.h"
 
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

  sync_horario();

  vTaskDelete(NULL);
}

void handler_webconfigwifi(){
  //colocar as condicoes do server.on em start_server aqui
  return;
}

void start_server(){
  
  //paginas
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (auth == 0) request->send(LITTLEFS, "/no_auth.html");
    if (auth == 1) request->send(LITTLEFS, "/ok_auth.html");
  });
  
  //estilos
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/favicon.ico");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/style.css");
  });
  server.on("/files/font-awesome.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/font-awesome.min.css");
  });
  server.on("/files/roboto.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/roboto.css");
  });

  //imagens
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

  //fontes
  server.on("/files/Metropolis-Light.otf", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LITTLEFS, "/files/icon_consulta.png");
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

  server.on("/connecting", HTTP_GET, [](AsyncWebServerRequest *request){
    if(connection_status == "ok"){
      request->send(LITTLEFS, "/connected.html");
    }
    if(connection_status == "fail"){
      request->send(LITTLEFS, "/invalid_network.html");
    }
    if(connection_status == "connecting"){
      request->send(LITTLEFS, "/connecting.html");
    }
    if(connection_status == "null"){
      request->redirect("/");
    }
  });
  
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
    Serial.print("\n[WEB] Numero de parametros recebidos: ");
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
 
        AsyncWebParameter* p = request->getParam(i);
        if (p->name() == "key" && auth == 0){

            if (p->value() == key){

                auth=1; //autentica
                //NOTA: criar handler para essa tarefa e matar ela aqui, antes de começar outra pois se começar mais uma o esp crasha
                xTaskCreatePinnedToCore(
                  auth_timer,       //task function
                  "AuthTimer",      //task name
                  2048,             //stack size
                  NULL,             //parameters to pass to the task
                  1,                //priority
                  NULL,             //task handle
                  0);               //cpu id

                Serial.println("[QUERY RECEBIDA]");
                Serial.println("[WEB] Autenticado. Iniciando timer de 1 minuto.\n");
            }
        }

        if (p->name() == "ssid_host" && auth == 1){

            Serial.println("[QUERY RECEBIDA]");
            Serial.print("Parametro: ");
            Serial.println(p->name());

            ssid_host = p->value(); //pega o valor do parametro

            Serial.print("Valor: ");
            Serial.println(ssid_host);
            Serial.println(" ");
        }

        if (p->name() == "pwd_host" && auth == 1){

            Serial.println("[QUERY RECEBIDA]");
            Serial.print("Parametro: ");
            Serial.println(p->name());

            pwd_host = p->value(); //pega o valor do parametro

            Serial.print("Valor: ");
            Serial.println(pwd_host);
            Serial.println(" ");
        }
    }

    if(auth == 1 && ssid_host != "null" && pwd_host != "null" && connection_status!="ok"){
        connection_status="connecting";
        request->redirect("/connecting");
        delay(1000);
        xTaskCreatePinnedToCore(
          conecta_host,     //task function
          "ConectaWifi",    //task name
          8192,             //stack size
          NULL,             //parameters to pass to the task
          1,                //priority
          NULL,     //task handle
          0);               //cpu id
    }
    if (auth == 0){
      request->send(LITTLEFS, "/invalid_auth.html");
      }

  });
  
  Serial.println("[WEB] Ligando o ESPAsyncWebServer");
  server.begin();

  return;
}

void setup(){

  //inicia o Serial para debug
  Serial.begin(115200);  

  // Inicializa LITTLEFS
  

  if(!LITTLEFS.begin(true)){
    Serial.println("[SISTEMA] An error has occurred while mounting LITTLEFS.");
    return;
  }
  else{
    Serial.println("[SISTEMA] LITTLEFS montado. Lista de arquivos:");
  }

  listDir(LITTLEFS, "/", 3);
  
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
  
  WiFi.onEvent(WiFiEvent);
  
  start_server();

}
 
void loop(){

}