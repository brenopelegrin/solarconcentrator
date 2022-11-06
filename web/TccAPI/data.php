<?php
ini_set('memory_limit', '1024M');
include("conectionmysql.php");
ini_set('display_errors', '0');


/*
inputs -> 
    typeTime -> lastData -> quant -> max 50
             -> lastMinute
             -> custom -> iniDate 
                       -> endDate
                       -> resolution (endDate - iniDate) -> 3 days -> 10 min, 30 min, 1h
                                                         -> 2 days -> 5 min, 10 min, 30 min
                                                         -> 1 day -> 1 min, 2 min, 3 min
                                                         -> 0 day -> error
                       

    typeData -> all
             -> temperature -> sensor -> T1, T2, T3, T4, T5, T6, T7, T8, T9, T10
             -> flow (vazão) -> sensor -> V1
             -> luminosity -> sensor -> L1 -> falta implementar
             -> velocityWind -> sensor -> AN1
             -> irradiance -> sensor -> IR1

    

*/


class InfoData {

    var $dbaccess = NULL;
    var $contentArray = NULL;

    function __construct()
    {
        $this->dbaccess = createAccessDB();
    }

    public function closeConnectionDB()
    {
        $this->dbaccess->close();
    }

    private function printJson($codeStatus, $content)
    {
        $this->contentArray = array(
            "code" => $codeStatus,
            "content" => $content
        );
        header('Content-Type: application/json');
		echo json_encode($this->contentArray);
    }

    public function resolveGetData() 
    {
        if (isset($_POST["typeTime"]) && isset($_POST["typeData"])) {
            $typeTime = trim(strip_tags($_POST['typeTime']));
            $typeData = trim(strip_tags($_POST['typeData']));
        } 
        else {
            $this->printJson(400, "Error: Nonexistent Type Valor");
            exit(); 
        }
        if (!($typeTime == "lastData" || $typeTime == "lastMinute" || $typeTime == "custom")) {
            $this->printJson(400, "Error: No value correct option");
            exit(); 
        }

        $sensor = NULL;

        if ($typeTime == "lastData") {
            $sensor = $this->resolveWhichSensor($typeData);
            $this->getLastData($typeData, $sensor);
        }
        else if ($typeTime == "lastMinute") {
            $sensor = $this->resolveWhichSensor($typeData);
            $this->getLastMinute($typeData, $sensor);
        }
        else if ($typeTime == "custom") {
            $sensor = $this->resolveWhichSensor($typeData);
            $this->getCustomData($typeData, $sensor);
        }
        
    }

    private function resolveWhichSensor($typeData) 
    {
        
        
        $temperatureSensors = ["T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9", "T10"];

        $arraySelectSensors = [];

        $continuaVeri = true;
        if ($typeData == "all") {
            $arraySelectSensors = ["T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9", "T10", "AN1", "L1", "IR1", "V1"];
        }
        else {
            if (!isset($_POST['sensor'])) {
                $this->printJson(400, "Empty Sensor");
                exit();
            }
            $sensorInput = trim(strip_tags($_POST['sensor']));
            $arraySensor = explode(',', $sensorInput);

            $arraySensor = array_unique($arraySensor);
            for ($i = 0; $i < count($arraySensor) && $continuaVeri; $i++){
                if ($typeData == "temperature") {
                    foreach($temperatureSensors as $completeTempSensor){
                        if ($arraySensor[$i] == $completeTempSensor) {
                            array_push($arraySelectSensors, $arraySensor[$i]);
                        }
                    }
                } 
                else if ($typeData == "flow") {
                    if ($arraySensor[$i] == "V1") {
                        $continuaVeri = false;
                        array_push($arraySelectSensors, $arraySensor[$i]);
                    }
                }
                else if ($typeData == "luminosity") {
                    if ($arraySensor[$i] == "L1") {
                        $continuaVeri = false;
                        array_push($arraySelectSensors, $arraySensor[$i]);
                    }
                } 
                else if ($typeData == "velocityWind") {
                    if ($arraySensor[$i] == "AN1") {
                        $continuaVeri = false;
                        array_push($arraySelectSensors, $arraySensor[$i]);
                    }
                } 
                else if ($typeData == "irradiance") {
                    if ($arraySensor[$i] == "IR1") {
                        $continuaVeri = false;
                        array_push($arraySelectSensors, $arraySensor[$i]);
                    }
                }
            }
        }
        

        if (empty($arraySelectSensors)){
            $this->printJson(400, "Errors Sensors");
            exit(); // sensores não validos
        }

        return $arraySelectSensors;
    }

    private function resolveWhichTime() 
    {
        // tipo de input esperado -> 2021-06-05
        if (!(isset($_POST["initDate"]) || isset($_POST["endDate"]) && isset($_POST["resolution"]))) {
            exit(); // sem quantidade expecificada
        }

        date_default_timezone_set('America/Sao_Paulo'); // não apague isso, aqui a cobra fuma

        $initDate = trim(strip_tags($_POST['initDate']));
        $endDate = trim(strip_tags($_POST['endDate']));

        $datetimeInit = new DateTime($initDate); //start time
        $datetimeEnd = new DateTime($endDate); //end time

        $resolution = trim(strip_tags($_POST['resolution']));
        $interval = date_diff($datetimeEnd, $datetimeInit);

        $interval = intval($interval->format("%d"));

        $now = time(); // or your date as well
        $timestampIni = strtotime($initDate);
        $datediffInit = ceil(($now - $timestampIni) / 86400); // difference days between now and input date

        $timestampEnd = strtotime($endDate);
        $datediffEnd = ceil(($now - $timestampEnd) / 86400); // difference days between now and input date
		

        //$timestampEnd = strtotime('+1 days', $timestampEnd); // adidciona 1 dia ao timestamp final
        if (!($interval <= 3)) {
			var_dump($interval, $datediffInit, $datediffEnd);
			echo "FORA DO RANGE";
			die();
        }

        $minutevalor = NULL;
        // interval of day
        if ($interval == 3) {
            if ($resolution == "10min") {
                $minutevalor = 10;
            }
            else if ($resolution == "30min") {
                $minutevalor = 30;
            }
            else if ($resolution == "1h"){
                $minutevalor = 60;
            }
            else {
                $this->printJson(400, "Error: Interval out of scope");
                exit();
            }
        } else if ($interval == 2) {
            if ($resolution == "5min") {
                $minutevalor = 5;
            }
            else if ($resolution == "10min") {
                $minutevalor = 10;
            } 
            else if ($resolution == "30min") {
                $minutevalor = 30;
            }
            else {
                $this->printJson(400, "Error: Interval out of scope");
                exit();
            }
        } else {
            if ($resolution == "1min") {
                $minutevalor = 1;
            } 
            else if ($resolution == "2min") {
                $minutevalor = 2;
            } 
            else if ($resolution == "3min") {
                $minutevalor = 3;
            }
            else {
                $this->printJson(400, "Error: Interval out of scope");
                exit();
            }
        }

        return [$resolution, $initDate, $endDate, $datetimeInit, $datetimeEnd, $interval, $timestampIni, $timestampEnd, $minutevalor];
    }

    private function getCustomData($typeData, $sensors)
    {
        $timeArray = $this->resolveWhichTime();
        $fisrtPartQuery = "SELECT id, tempo";
        $querySensors = NULL;
        foreach ($sensors as $sensor) {
            $querySensors = $querySensors . ", {$sensor}";
        }
        $tempSeconds = $timeArray[8] * 60;

        $tempoInicial = $timeArray[6] - 10;
        $tempoFinal = $timeArray[7] + 10;

        $sqlquery = $fisrtPartQuery.$querySensors." FROM id16791887_tcc.dados WHERE tempo BETWEEN {$tempoInicial} AND {$tempoFinal} AND tempo % {$tempSeconds} = 0 GROUP BY tempo";

        $result = $this->dbaccess->query($sqlquery);

        if (!$result) {
            trigger_error('Invalid query: ' . $this->dbaccess->error);
        }
        else if ($result->num_rows > 0) {     // output data of each row   
            $this->contentArray["code"] = 200;

            $count = 0;
            $tempSensors = array();
            while ($row = $result->fetch_assoc()) {
				// não FUÇE
                unset($tempSensors);
                $tempSensors = array(); // limpra array de sensores
				
                foreach ($sensors as $sensor) {
                    $tempSensors += array($sensor => $row[$sensor]);
                    $count = $count + 1;
                }

                $tempStemp = intval($row["tempo"]);
                $date = new DateTime("@$tempStemp");

                $this->contentArray["content"][] = array(
                    "id" => $row["id"], "tempo" => $row["tempo"], "data" =>
                    $date->format('Y-m-d H:i:s'), "sensores" =>
                    $tempSensors
                );
            }
        } else {
            $this->contentArray = array(
                "code" => 200,
                "content" => "No results"
            );
        }

    }

    private function getLastMinute($typeData, $sensors)
    {
        $fisrtPartQuery = "SELECT id, tempo";
        $querySensors = NULL;
        foreach ($sensors as $sensor) {
            $querySensors = $querySensors . ", {$sensor}";
        }
        $sqlquery = $fisrtPartQuery.$querySensors." FROM id16791887_tcc.dados WHERE tempo = (SELECT MAX(tempo) FROM id16791887_tcc.dados)";
        $result = $this->dbaccess->query($sqlquery);

        if ($result->num_rows > 0) {     // output data of each row   
            $this->contentArray["code"] = 200;

            $count = 0;
            $tempSensors = array();
            while ($row = $result->fetch_assoc()) {
                // não FUÇE
                unset($tempSensors);
                $tempSensors = array(); // limpra array de sensores
                foreach ($sensors as $sensor) {
                    $tempSensors += array($sensor => $row[$sensor]);
                    $count = $count + 1;
                }

                $tempStemp = intval($row["tempo"]);
                $date = new DateTime("@$tempStemp");

                $this->contentArray["content"][] = array(
                    "id" => $row["id"], "tempo" => $row["tempo"], "data" =>
                    $date->format('Y-m-d H:i:s'), "sensores" =>
                    $tempSensors
                );
            }
        } else {
            $this->contentArray = array(
                "code" => 200,
                "content" => "No results"
            );
        }
    }

    private function getLastData($typeData, $sensors)
    {
        if (!isset($_POST["quant"])) {
            exit(); // sem quantidade expecificada
        }

        $quantData = trim(strip_tags($_POST['quant']));

        if (!($quantData <= 1440 && $quantData > 0)) { // 1440 minutos -> 24 horas
            $this->printJson(400, "Error: Out of range quant");
            exit(); // fora do range especificado
        }

        $fisrtPartQuery = "SELECT id, tempo";
        $querySensors = NULL;

        foreach ($sensors as $sensor) {
            $querySensors = $querySensors . ", {$sensor}";
        }

        //date_default_timezone_set('America/Sao_Paulo');

        $segundosLastData = (intval($quantData) * 60) - 10;
        $sqlquery = $fisrtPartQuery.$querySensors." FROM id16791887_tcc.dados WHERE tempo BETWEEN ((SELECT MAX(tempo) FROM id16791887_tcc.dados) - {$segundosLastData}) AND (SELECT MAX(tempo) FROM id16791887_tcc.dados)";
        
        $result = $this->dbaccess->query($sqlquery);
        if (!$result) {
            trigger_error('Invalid query: ' . $this->dbaccess->error);
        }
        else if ($result->num_rows > 0) {     // output data of each row   
            $this->contentArray["code"] = 200;

            $count = 0;
            $tempSensors = array();
            while ($row = $result->fetch_assoc()) {  
                // não FUÇE
                unset($tempSensors);
                $tempSensors = array(); // limpra array de sensores      
                foreach ($sensors as $sensor) {
                    $tempSensors += array($sensor => $row[$sensor]);
                    $count = $count + 1;
                }

                $tempStemp = intval($row["tempo"]);
                $date = new DateTime("@$tempStemp");

                $this->contentArray["content"][] = array(
                    "id" => $row["id"], "tempo" => $row["tempo"], "data" =>
                    $date->format('Y-m-d H:i:s'), "sensores" =>
                    $tempSensors
                );
            }
        } else {
            $this->contentArray = array(
                "code" => 200,
                "content" => "No results"
            );
        }
    }
}