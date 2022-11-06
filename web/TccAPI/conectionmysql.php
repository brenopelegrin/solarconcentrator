<?php

function createAccessDB() {
  $servername = "localhost";
  $username = "root";
  $password = "";

  return new mysqli($servername, $username, $password);
}
// Create connection
