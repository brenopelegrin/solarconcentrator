# TccAPI

> Esse diretório é uma cópia do repositório [TccAPI](https://github.com/Stecken/TccAPI).

 API of TCC


# Documentação para o uso correto da API

## Consumo da API

Quando for consumir os dados da API, realize uma requesição com o método POST form encooded.

Campo | Descrição
------|------------
typeTime | Tipo de tempo que queira receber os dados, podendo ser `lastData` ou `custom`.
typeData | Tipo de dados que queira receber, podendo ser `all`, `temperature`, `flow`, `velocityWind` ou `irradiance`.
sensor | Os sensores que queira receber, de acordo com o campo typeData.



### Sensores de acordo com o campo typeData.
Saliento, é possível pegar informações de um, ou quantos sensores quiser, exceto tipos de dados com `*`.

Tipos de dados | Sensores
------|------------
*`all` | Response com dados de todos os sensores.
`temperature` | `T1`, `T2`, `T3`, `T4`, `T5`, `T6`, `T7`, `T8`, `T9`, `T10`.
`flow` | `V1`.
`velocityWind` | `AN1`.
`irradiance` | `IR1`.


Exemplos de requesição

````
POST / HTTP/1.1
Host: {Sua host}
Content-Type: application/x-www-form-urlencoded

typeTime=lastData&typeData=all
````

````
POST / HTTP/1.1
Host: {Sua host}
Content-Type: application/x-www-form-urlencoded

typeTime=lastData&typeData=temperature&sensor=T1,T2
````

Exemplo de Response

```javascript
{
  "a construir"
}
```

## Envio de dados

Quando for consumir os dados da API, realize uma requesição com o método GET.

Campo | Descrição
------|------------
key | Chave de acesso configurada pelo o administrador da API, somente quem possuir poderá executar o request.
tempo | Timestamp da requesição. Não cause duplicidade de timestamp.
t1 | Sensor 1 de temperatura. *dados em float.
t2 | Sensor 2 de temperatura. *dados em float.
t3 | Sensor 3 de temperatura. *dados em float.
t4 | Sensor 4 de temperatura. *dados em float.
t5 | Sensor 5 de temperatura. *dados em float.
t6 | Sensor 6 de temperatura. *dados em float.
t7 | Sensor 7 de temperatura. *dados em float.
t8 | Sensor 8 de temperatura. *dados em float.
t9 | Sensor 9 de temperatura. *dados em float.
t10 | Sensor 10 de temperatura. *dados em float.
vento | Sensor de velocidade do vento. *dados em float.
luminosidade | Sensor de luminosidade. *dados em float.
radiacao | Sensor de irradiação. *dados em float.
vazao | Sensor de vazao. *dados em float.

Exemplos de requesição

````
GET /?key=EXEMPLO&tempo=1980801&t1=3.4&t2=3.9&t3=3.1&t4=2.9&t5=2.3&t6=2.1&t7=2.0&t8=1.2&t9=1.7&t10=1.9&vento=1.1&luminosidade=0.9&radiacao=5.6&vazao=3.7 HTTP/1.1
Host: {Sua host}
````

Exemplo de Response

```javascript
{
  "a construir"
}
```
