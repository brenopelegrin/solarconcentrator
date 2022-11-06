<?php
	//include("permissionapi.php");
	include("data.php");


	$dados = NULL;
	
	$request_method = $_SERVER["REQUEST_METHOD"];
	$objJsonResult = array("code" => NULL, "content" => NULL);
	
	switch($request_method)
	{
		case 'GET':
			$objJsonResult["code"] = 200;
			header('Content-Type: application/json');
			echo json_encode($objJsonResult);
			break;
		case 'POST':
			$dados = new InfoData();
			$dados->resolveGetData();
			$dados->closeConnectionDB();
			header('Content-Type: application/json');
			echo json_encode($dados->contentArray); // transforma array em json e manda ao requerente
			break;
		default:
			// Invalid Request Method
			header("HTTP/1.0 405 Method Not Allowed");
			header('Content-Type: application/json');
			echo json_encode(array("code" => 405, "content" => NULL));
			break;
	}



	// Close database connection