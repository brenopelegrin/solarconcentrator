#include <Arduino.h>
#include "FS.h"
#include <LITTLEFS.h>

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listando diretorio: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- falha ao abrir o diretorio");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - nao e um diretorio");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIRETORIO : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  ARQUIVO: ");
            Serial.print(file.name());
            Serial.print("\tTAMANHO: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Criando diretorio: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Diretorio criado");
    } else {
        Serial.println("mkdir falhou");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removendo diretorio: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Diretorio removido");
    } else {
        Serial.println("rmdir falhou");
    }
}

void printFile(fs::FS &fs, const char * path){
    Serial.printf("Lendo arquivo: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- falha ao abrir o arquivo para leitura");
        return;
    }

    Serial.println("- leitura do arquivo:");
    while(file.available()){
        Serial.write(file.read());
    }
    Serial.print("\n");
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Escrevendo arquivo: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- falha ao abrir o arquivo para escrita");
        return;
    }
    if(file.print(message)){
        Serial.println("- arquivo escrito");
    } else {
        Serial.println("- escrita falhou");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Acrescentando ao arquivo %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- falha ao abrir o arquivo para acrescentar");
        return;
    }
    if(file.print(message)){
        Serial.println("- mensagem acrescentada");
    } else {
        Serial.println("- acrescimo falhou");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renomeando o arquivo %s para %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- arquivo renomeado");
    } else {
        Serial.println("- arquivo renomeado");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Excluindo arquivo: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- arquivo excluido");
    } else {
        Serial.println("- falha ao excluir");
    }
}