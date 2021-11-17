#define LED_BUILTIN 2
#define BUZZER_PIN 18
#define ONE_WIRE_BUS 16
#define PINO_ANEMOMETRO 27
#define PINO_VAZAO 25
#define EN_MOTOR 4
#define PINO_PIRANOMETRO 26
#define DIR_MOTOR 0
#define PUL_MOTOR 2

unsigned int counterANEMOMETRO_var = 0;        //Contador para o sensor de anemometro
volatile byte counterVAZAO_var = 0;            //Contador para o sensor de vazao

void counterANEMOMETRO() {//incrementa contador
  counterANEMOMETRO_var++;
} 

void IRAM_ATTR counterVAZAO(){//incrementa contador
  counterVAZAO_var++;
}

void configura_gpio(){
    pinMode(PINO_VAZAO, INPUT_PULLUP);             //Sensor vazão
    attachInterrupt(digitalPinToInterrupt(PINO_VAZAO), counterVAZAO, FALLING); //interrupção sensor vazão

    pinMode(PINO_ANEMOMETRO, INPUT_PULLUP);        //configura o digital 2 como entrada
    pinMode(EN_MOTOR, OUTPUT);
    pinMode(DIR_MOTOR, OUTPUT);
    pinMode(PUL_MOTOR, OUTPUT);
}