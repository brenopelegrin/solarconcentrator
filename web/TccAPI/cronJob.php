<?php

    include("conectionmysql.php");

    $dbaccess = createAccessDB();

    $dbaccess->query("TRUNCATE TABLE id16791887_tcc.dados");    

    $dbaccess->close();
