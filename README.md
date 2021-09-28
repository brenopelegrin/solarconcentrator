# Concentrador Solar

![Logo](https://concentradorsolar.vercel.app/files/images/gota.png)

## O projeto

Nesse repositório são armazenados os arquivos utilizados na *construção do seguinte Trabalho de Conclusão de Curso:

> **ANÁLISE TEÓRICA DE UM COLETOR SOLAR CONCENTRADOR CILÍNDRICO PARABÓLICO POR TUBO EVACUADO COM RASTREAMENTO SOLAR E INTERFACE WEB**


O projeto foi desenvolvido pelos seguintes alunos do Curso Técnico em Mecatrônica Integrado ao Ensino Médio do IFSP Campus Catanduva:

- Alison José Pereira Neri
- [Breno Henrique da Silva](http://github.com/br3n0henrique "Breno Henrique da Silva")
- Paulo Henrique Nascimento Filho
- Rafael Garbo da Silva
- Samuel Lins Gomes
- [Yuri Martins](http://github.com/Stecken "Yuri Martins")

------------

## Diretórios
###  /solarconcentrator/web/
Nesse diretório são dispostos os arquivos do servidor web hospedado na internet que recebe os dados do concentrador solar.


### /solarconcentrator/esp32/
Nesse diretório é disposto o código fonte do ESP32, responsável por controlar o concentrador solar e enviar os dados à instância web.
#### main.cpp
Nesse arquivo é realizada a sequência de inicialização de rede, sensores e sistema de arquivos do microcontrolador em ```void setup()``` e também, a configuração e execução do servidor web interno do ESP32, utilizado para configurar a rede Wi-Fi, dados geográficos, servidor NTP e variáveis internas.

### /solarconcentrator/schematic/
Nesse diretório são dispostos os diagramas esquemáticos e montagem da estrutura mecânica e elétrica do projeto.

