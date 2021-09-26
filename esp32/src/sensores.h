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

int num_dallas = 0;

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