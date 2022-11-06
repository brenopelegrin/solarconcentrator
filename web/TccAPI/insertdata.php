<?php


define("SECRET_KEY", "");
// inserir uma secret key para o esp32

    include("conectionmysql.php");
    $request_method = $_SERVER["REQUEST_METHOD"];

    $contentArray = array();

    switch ($request_method) {
        case 'GET':
            $dbaccess = createAccessDB();
            getValor(); // pega os valores e coloca em um array chave-valor
            $dbaccess->close();
            header('Content-Type: application/json');
            echo json_encode($contentArray); // transforma array em json e manda ao requerente
            break;
        default:
            // Invalid Request Method
            header("HTTP/1.0 405 Method Not Allowed");
            header('Content-Type: application/json');
            echo json_encode(array(
                "code" => 405,
                "content" => NULL
            ));
            break;
    }

    function getValor()
    {
        global $contentArray, $dbaccess;

        // possui sql injection
        if (isset($_GET["key"])) {
            if ($_GET["key"] == SECRET_KEY) {
                $tempo = $_GET["tempo"];
                $T1 = $_GET["t1"];
                $T2 = $_GET["t2"];
                $T3 = $_GET["t3"];
                $T4 = $_GET["t4"];
                $T5 = $_GET["t5"];
                $T6 = $_GET["t6"];
                $T7 = $_GET["t7"];
                $T8 = $_GET["t8"];
                $T9 = $_GET["t9"];
                $T10 = $_GET["t10"];
                $vento = $_GET["vento"];
                $luminosidade = $_GET["luminosidade"];
                $radiacao = $_GET["radiacao"];
                $vazao = $_GET["vazao"];

                $format = "INSERT INTO id16791887_tcc.dados VALUES(0, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)";
                $sqlQuery = sprintf(
                    $format,
                    $tempo,
                    $T1,
                    $T2,
                    $T3,
                    $T4,
                    $T5,
                    $T6,
                    $T7,
                    $T8,
                    $T9,
                    $T10,
                    $vento,
                    $luminosidade,
                    $radiacao,
                    $vazao
                );

                $result = $dbaccess->query($sqlQuery);

                $contentArray["code"] = 200;
                $contentArray["info"] = $result;
            }
            else {
                $contentArray["code"] = 403;
                $contentArray["info"] = "BANIDO";
            }
            
        } else {
            $contentArray["code"] = 400;
        }
    }