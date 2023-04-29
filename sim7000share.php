<?php

/*
save data from lilygo sim7000g to mysql
*/

//============configuration===========================================
//##########################################################
//server
  $servername = "unclephysics.com.ng";
//Database name
  $dbname = "unclephy_sim7000share";
//Database user
  $username = "unclephy_sim7000";
//Database user password
  $password = "BobJohnSon2841";
//##########################################################  
//local api key - can be anything but must match esp8266 code
  $api_key_value = "tPmAT5Ab3j7F9"; // must match IDE code
//====================================================================
// Create connection
      $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
      if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
      } 

$api_key = $temperature = $humidity = $dewpoint = $pressure = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $temperature = test_input($_POST["temperature"]);
        $humidity = test_input($_POST["humidity"]);
        $dewpoint = test_input($_POST["dewpoint"]);
        $pressure = test_input($_POST["pressure"]);
        
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        //get php date and time 
    date_default_timezone_set("Africa/Lagos");
    $sendtime =  date("Y-m-d H:i:s");
    
    
    //send data to mysql file    
        $sql = "INSERT INTO sim7000Share (temperature, humidity, dewpoint, pressure, sendtime)
    VALUES ('" . $temperature . "','" . $humidity . "', '" . $dewpoint . "', '" . $pressure . "', '" . $sendtime . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}