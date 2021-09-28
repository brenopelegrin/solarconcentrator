#define ONE_WIRE_BUS 21
#define TEMPERATURE_PRECISION 9

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//endereço dos sensores
DeviceAddress sens_t1 = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC };
DeviceAddress sens_t2 = { 0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27 };
DeviceAddress sens_t3 = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };
DeviceAddress sens_t4 = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC };
DeviceAddress sens_t5 = { 0x28, 0x60, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27 };
DeviceAddress sens_t6 = { 0x28, 0x50, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };
DeviceAddress sens_t7 = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x03, 0xEC };
DeviceAddress sens_t8 = { 0x28, 0x60, 0x64, 0x12, 0x3C, 0x5C, 0x2F, 0x27 };
DeviceAddress sens_t9 = { 0x28, 0x50, 0x64, 0x12, 0x3F, 0xFD, 0x70, 0xC6 };
DeviceAddress sens_t10 = { 0x28, 0x20, 0x64, 0x12, 0x3F, 0xFD, 0x70, 0xC6 };

DeviceAddress tempDeviceAddress;

float vento = 8.3104;
float luminosidade = 1274.1142;
float radiacao = 1123.2324;
float vazao = 0.12345678;

int num_dallas = 0;
int vazao_status = 0;
int anenometro_status = 0;
int piranometro_status = 0;

float t1 = 0.0;
float t2 = 0.0;
float t3 = 0.0;
float t4 = 0.0;
float t5 = 0.0;
float t6 = 0.0;
float t7 = 0.0;
float t8 = 0.0;
float t9 = 0.0;
float t10 = 0.0;

//tutorial indiano
#define LED_BUILTIN 2
#define SENSOR  27

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount = 0;
byte pulse1Sec = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

//falta iniciar a tarefa
void read_sensores(void * parameters){
    while(1){
        currentMillis = millis();
        if (currentMillis - previousMillis > interval) {
            pulse1Sec = pulseCount;
            pulseCount = 0;

            // Because this loop may not complete in exactly 1 second intervals we calculate
            // the number of milliseconds that have passed since the last execution and use
            // that to scale the output. We also apply the calibrationFactor to scale the output
            // based on the number of pulses per second per units of measure (litres/minute in
            // this case) coming from the sensor.
            flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor; //vazao em L/min
            previousMillis = millis();

            // Divide the flow rate in litres/minute by 60 to determine how many litres have
            // passed through the sensor in this 1 second interval, then multiply by 1000 to
            // convert to millilitres.
            flowMilliLitres = (flowRate / 60) * 1000; //vazao em mL/s

            // Add the millilitres passed in this second to the cumulative total
            totalMilliLitres += flowMilliLitres; //total de mL passados (tudo)
        }

        //le os sensores Dallas
        sensors.requestTemperatures();
        t1 = sensors.getTempC(sens_t1);
        t2 = sensors.getTempC(sens_t2);
        t3 = sensors.getTempC(sens_t3);
        t4 = sensors.getTempC(sens_t4);
        t5 = sensors.getTempC(sens_t5);
        t6 = sensors.getTempC(sens_t6);
        t7 = sensors.getTempC(sens_t7);
        t8 = sensors.getTempC(sens_t8);
        t9 = sensors.getTempC(sens_t9);
        t10 = sensors.getTempC(sens_t10);
        
    }
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void configSensores(){
    //inicia a interface 1-wire
    sensors.begin();
    num_dallas = sensors.getDeviceCount();
    Serial.print("[I/O] Definindo a resolução para ");
    Serial.print(TEMPERATURE_PRECISION, DEC);
    //configura a resolução dos sensores cadastrados
    sensors.setResolution(sens_t1, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t2, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t3, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t4, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t5, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t6, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t7, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t8, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t9, TEMPERATURE_PRECISION);
    sensors.setResolution(sens_t10, TEMPERATURE_PRECISION);
    Serial.println(" bits.");
    Serial.printf("[I/O] Listando sensores DS18B20 conectados.\n");
    Serial.printf("--- Iniciando listagem de sensores DS18B20 ---\n");
    int i = 0;
    // mostra informações sobre cada sensor conectado
    for(i=0; i<num_dallas; i++){
        if(sensors.getAddress(tempDeviceAddress, i)){
            //verifica se o endereço do sensor i é igual ao endereço cadastrado pra cada sensor Tj
            if(tempDeviceAddress == sens_t1) Serial.printf("[SENSOR T%d (ID %d)]:\n", 1, i);
            if(tempDeviceAddress == sens_t2) Serial.printf("[SENSOR T%d (ID %d)]:\n", 2, i);
            if(tempDeviceAddress == sens_t3) Serial.printf("[SENSOR T%d (ID %d)]:\n", 3, i);
            if(tempDeviceAddress == sens_t4) Serial.printf("[SENSOR T%d (ID %d)]:\n", 4, i);
            if(tempDeviceAddress == sens_t5) Serial.printf("[SENSOR T%d (ID %d)]:\n", 5, i);
            if(tempDeviceAddress == sens_t6) Serial.printf("[SENSOR T%d (ID %d)]:\n", 6, i);
            if(tempDeviceAddress == sens_t7) Serial.printf("[SENSOR T%d (ID %d)]:\n", 7, i);
            if(tempDeviceAddress == sens_t8) Serial.printf("[SENSOR T%d (ID %d)]:\n", 8, i);
            if(tempDeviceAddress == sens_t9) Serial.printf("[SENSOR T%d (ID %d)]:\n", 9, i);
            if(tempDeviceAddress == sens_t10) Serial.printf("[SENSOR T%d (ID %d)]:\n", 10, i);

            //se nao for igual a nenhum Tj, sensor não cadastrado.
            if(tempDeviceAddress != sens_t1 && tempDeviceAddress != sens_t2 && tempDeviceAddress != sens_t3 && tempDeviceAddress != sens_t4 && tempDeviceAddress != sens_t5 && tempDeviceAddress != sens_t6 && tempDeviceAddress != sens_t7 && tempDeviceAddress != sens_t8 && tempDeviceAddress != sens_t9 && tempDeviceAddress != sens_t10){
                Serial.printf("[SENSOR T? (ID %d) - não cadastrado]:\n", i);
                sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
            }
		    Serial.printf("Endereço: ");
		    printAddress(tempDeviceAddress);
		    Serial.println();
            Serial.print("Resolução: ");
            Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
            Serial.println(" bits.");
		
	    }
        else{
            //sensor conectado mas não funciona
		    Serial.printf("[SENSOR DEFEITUOSO (ID %d)]:\n", i);
		    Serial.printf("Não foi possível detectar o endereço ou definir a resolução.");
	    }
    }
    Serial.printf("Total de sensores DS18B20 conectados: %d\n", num_dallas);
    Serial.printf("--- Fim da listagem ---\n");

    return;
}