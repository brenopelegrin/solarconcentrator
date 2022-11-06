# <img src="https://concentradorsolar.vercel.app/files/images/gota.png" width="80" height="80" /> Concentrador Solar

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

A API e a interface web do projeto foram desenvolvidas por [Yuri Martins](http://github.com/Stecken "Yuri Martins"), e os esquemáticos eletrônicos e os códigos de baixo nível do ESP32 foram desenvolvidos por [Breno Henrique da Silva](http://github.com/brenopelegrin "Breno Henrique da Silva"). A parte mecânica e análise óptica foram conduzidas pelo restante do grupo e a produção da monografia teve participação de todos os membros do grupo.

A [monografia](monografia.pdf "monografia") foi defendida no dia 30/11/2021 em reunião on-line do IFSP Câmpus Catanduva, sendo a banca avaliadora composta pelos membros:

- Elson Avallone (orientador)
- Pablo Sampaio Gomes Natividade (co-orientador)
- Aldo Yoshida Rigatti
- Cristiano Donizeti Ferrari
- Daniel Corrêa Lobato

No processo de correção e revisão da monografia e acompanhamento do desenvolvimento do projeto, estiveram envolvidos os docentes:
- Márcia Corrêa de Oliveira Mariano 
- Felipe Amélio de Lucena

Posteriormente, o link para o vídeo da defesa será adicionado aqui.

------------

## Partes do projeto
###  /solarconcentrator/web/
- Nesse diretório é disposta uma cópia dos respoitórios da [API](https://github.com/Stecken/TccAPI "API") e da [Interface web](https://github.com/Stecken/WebViewTCC "Interface Web") desenvolvidas por [Yuri Martins](http://github.com/Stecken "Yuri Martins"). 

### /solarconcentrator/esp32/
- Nesse diretório é disposto o código fonte do ESP32, responsável por controlar o concentrador solar e enviar os dados à instância web.

### /solarconcentrator/schematic/
- Nesse diretório são dispostos os diagramas esquemáticos e montagem da estrutura mecânica e elétrica do projeto.

| Parte | Documentação |
| ------ | ------ |
| web | [web/README.md](web/README.md) |
| esp32 | [esp32/README.md](esp32/README.md) |
| schematic | [schematic/README.md](schematic/README.md) |
